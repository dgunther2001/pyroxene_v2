#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <iostream>


namespace write_test_socket {
    void write_dummy_log_message(const char* message);
}