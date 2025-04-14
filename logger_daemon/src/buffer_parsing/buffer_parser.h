#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "log_writer.h"

namespace util_buffer_parser {
    enum log_level {
        TRACE = 1,
        DEBUG = 10,
        INFO = 20,
        WARN = 30,
        ERROR = 40,
        FATAL = 50
    };

    log_level get_error_level();
    std::string get_current_formatted_time();
    log_level parse_err_level_str(const char* err_level);
}

namespace buffer_parser {


    // what does this need to do?
    // have a reference to a log writer obj passed via the constructor from the orchestrator
    // have a pass parsed msg to log_writer function which takes a string (checks optional and passes if not std::nullopt)
    class buffer_parser_obj {
    public:
        buffer_parser_obj(log_writer::log_writer_obj& log_file_writer) : log_file_writer(log_file_writer) {}
        void enqueue_msg(std::string msg);
        void init_thread();
        void stop_thread();

        void wait_until_queue_empty();

        bool thread_active() { return is_thread_running; }

    private:
        std::vector<std::string> parse_char_buffer(std::string buffer, ssize_t buffer_length);
        std::optional<std::string> generate_output_message(std::vector<std::string> parsed_buffer);
        void enqueue_log_writer(std::optional<std::string> msg);

        std::queue<std::string> msgs_to_parse;
        log_writer::log_writer_obj& log_file_writer;
        
        
        void run_thread();

        std::condition_variable thread_active_condition_var;
        std::mutex thread_active_mutex;
        std::thread writing_thread;
        bool is_thread_running = false;
    };

}

