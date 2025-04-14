#include "daemon_orchestrator.h"

namespace daemon_orchestrator {
    daemon_orch_obj::daemon_orch_obj(const char* log_file_path) :
        log_writer(log_file_path),
        buffer_parser(log_writer),
        input_socket(buffer_parser)
    {}

    void daemon_orch_obj::start_threads() {
        log_writer.init_thread();
        buffer_parser.init_thread();

        input_socket.init_socket();
        input_socket.init_thread();
    }

    void daemon_orch_obj::kill_threads() {
        input_socket.stop_thread();
        input_socket.close_socket();

        buffer_parser.stop_thread();
        log_writer.stop_thread();
    }

    void daemon_orch_obj::wait_until_queues_empty() {
        buffer_parser.wait_until_queue_empty();
        log_writer.wait_until_queue_empty();
    }
}