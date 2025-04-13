#include "socket_comm.h"

namespace backend_socket {
    void open_sock(const char* SOCKET_PATH) {
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

        std::cout << "Waiting for connection on C++ backend socket " << SOCKET_PATH << "...\n";

        int client_fd = accept(sockfd, nullptr, nullptr);
        if (client_fd == -1) {
            perror("accept");
            return;
        }

        std::cout << "C++ socket opened and connected\n";

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
}