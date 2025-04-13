#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace backend_socket {
    void open_sock(const char* SOCKET_PATH);
}