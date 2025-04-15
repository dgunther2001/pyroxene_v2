#include <iostream>
#include <string>
#include <vector>

#include "daemon_orchestrator.h"
#include "buffer_parser_strategies/default_pyroxene_strategy/default_pyroxene_strategy.h"


#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <signal.h>
#include <fstream>

void write_dummy_log_message(const char* message) {
    const char* socket_path = std::getenv("PYROXENE_LOG_SOCKET_PATH");
    if (!socket_path) {
        std::cerr << "PYROXENE_LOG_SOCKET_PATH not set!\n";
        return;
    }

    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        return;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(client_fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(client_fd);
        return;
    }

    ssize_t bytes_written = write(client_fd, message, strlen(message));
    if (bytes_written == -1) {
        perror("write");
    } 

    close(client_fd);
}

std::vector<pid_t> wait_for_pid_list(const char* pid_path) {
    while (!std::filesystem::exists(pid_path) || std::filesystem::file_size(pid_path) == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::ifstream pid_file(pid_path);
    std::string pid_line;
    std::getline(pid_file, pid_line);
    pid_file.close();

    std::vector<pid_t> pids;
    std::stringstream ss(pid_line);
    std::string pid;

    while (std::getline(ss, pid, ':')) {
        pids.push_back(static_cast<pid_t>(std::stoi(pid)));
    }

    return pids;
}

void monitor_feeding_processes() {

    auto pids = wait_for_pid_list(std::getenv("PID_PATH"));

    while (true) {
        
        bool any_alive = false;
        for (pid_t pid : pids) {
            if (kill(pid, 0) == 0) { 
                any_alive = true;
                break;
            }
        }

        if (!any_alive) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}





int main() {
    const char* msg  = "DEBUG|LogInfo|Logger|C++|Logger Invoked|";
    const char* msg2 = "WARN|LogInfo|Logger|C++|Logger Invoked|";
    const char* msg3 = "ERROR|LogInfo|Logger|C++|Logger Invoked|";

    daemon_orchestrator::daemon_orch_obj orchestrator(std::getenv("PYROXENE_LOG_PATH"), &pyroxene_default_parser::pyroxene_default_parser);
    orchestrator.start_threads();
    orchestrator.log_orchestrator_info("DEBUG|LogInfo|Logger|C++|Orchestrator Initializing Threads|");

    for (int i = 0; i < 2; i ++) {
        write_dummy_log_message(msg);
        write_dummy_log_message(msg2);
        write_dummy_log_message(msg3);
    }
    
    std::thread monitor_thread(monitor_feeding_processes);
    monitor_thread.join();
    //orchestrator.wait_until_queues_empty();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    orchestrator.kill_threads();

    return 0;
}