#include "socket_comm.h"

int main() {
    backend_socket::open_sock(std::getenv("BACKEND_SOCKET_PATH"));
    return 0;
}