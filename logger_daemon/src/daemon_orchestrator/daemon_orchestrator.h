#pragma once

#include "input_socket.h"
#include "buffer_parser.h"
#include "log_writer.h"

namespace daemon_orchestrator {
    class daemon_orch_obj {
    public:
        daemon_orch_obj(const char* log_file_path);
        void start_threads();
        void kill_threads();
        void wait_until_queues_empty();

        // TEMPORARY => TO REMOVE!!!
        void enqueue_msg_to_socket(std::string msg) { input_socket.enqueue_buffer_parser(msg); /*buffer_parser.enqueue_msg(msg);*/ }

    private:
        log_writer::log_writer_obj log_writer;
        buffer_parser::buffer_parser_obj buffer_parser;
        input_socket::input_socket_obj input_socket;

    };
}