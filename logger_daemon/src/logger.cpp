#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

#define SOCKET_PATH "../../tmp/logger_daemon.sock"

void open_sock() {
    unlink(SOCKET_PATH);

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return;
    }

    sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return;
    }

    if (listen(sockfd, 1) == -1) {
        perror("listen");
        return;
    }

    std::cout << "Waiting for connection on Logger backend socket " << SOCKET_PATH << "...\n";

    int client_fd = accept(sockfd, nullptr, nullptr);
    if (client_fd == -1) {
        perror("accept");
        return;
    }

    std::cout << "Logger socket opened and connected\n";

    char buffer[1024];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        std::cout << "Received: " << std::string(buffer, bytes_read) << "\n";
    }

    close(client_fd);
    close(sockfd);
    unlink(SOCKET_PATH); 

    exit(0);
}

bool log_to_file(std::string file_path, std::string msg) {
    std::cout << "SOCKET PATH: " << file_path << std::endl;
    return true;
}


int main() {
    //open_sock();
    log_to_file(std::getenv("PYROXENE_LOG_SOCKET_PATH"), "daniel");
    return 0;
}