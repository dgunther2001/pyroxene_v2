#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "../../tmp/backend_pyroxene.sock"

namespace backend_socket {
    void open_sock();
}