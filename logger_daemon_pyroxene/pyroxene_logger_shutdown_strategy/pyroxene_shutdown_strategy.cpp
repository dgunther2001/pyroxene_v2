#include "pyroxene_shutdown_strategy.h"

namespace pyroxene_shutdown_strategy {
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
}