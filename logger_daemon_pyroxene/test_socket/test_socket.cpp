#include "test_socket.h"

namespace write_test_socket {
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
}