#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

#include "buffer_parser.h"

namespace input_socket {
    class input_socket_obj {
    public:
        input_socket_obj(buffer_parser::buffer_parser_obj& buffer_parser) : buffer_parser(buffer_parser) {}
        void init_socket();
        void read_socket();
        void close_socket();

        void init_thread();
        void stop_thread();

        bool thread_active() { return is_thread_running; }

        // MAKE PRIVATE LATER
        void enqueue_buffer_parser(std::string msg);

    private:
        int sockfd = -1;
        std::string SOCKET_PATH;


        buffer_parser::buffer_parser_obj& buffer_parser;

        void run_thread();

        std::condition_variable thread_active_condition_var;
        std::mutex thread_active_mutex;
        std::thread writing_thread;
        bool is_thread_running = false;
    };
}