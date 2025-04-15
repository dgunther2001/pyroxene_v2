#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <thread>

// TO REMOVE
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unordered_map>

using parser_strategy = std::function<std::optional<std::string>(const std::string&)>;

namespace util_buffer_parser {
    std::string get_current_formatted_time();

    enum log_level {
        TRACE = 1,
        DEBUG = 10,
        INFO = 20,
        WARN = 30,
        ERROR = 40,
        FATAL = 50
    };

    log_level get_error_level();
    log_level parse_err_level_str(const std::string& msg);

    enum valid_log_fields {
        LOG_LEVEL=1,
        LOG_TYPE=10,
        COMPONENT=20,
        LANGUAGE=30,
        MESSAGE=40,
        INVALID=100
    };

    valid_log_fields get_log_field(const std::string& log_field);

}

namespace buffer_parser {


    // what does this need to do?
    // have a reference to a log writer obj passed via the constructor from the orchestrator
    // have a pass parsed msg to log_writer function which takes a string (checks optional and passes if not std::nullopt)
    class buffer_parser_obj {
    public:
        buffer_parser_obj(std::function<void(std::string)> enqueue_to_log_writer_callback, 
                          std::function<void(std::string)> log_self_callback, 
                          std::function<bool()> log_writer_thread_active_callback,
                          parser_strategy parsing_strategy
                         ) : 
                         enqueue_to_log_writer_callback(enqueue_to_log_writer_callback),
                         log_self_callback(log_self_callback),
                         log_writer_thread_active_callback(log_writer_thread_active_callback),
                         parsing_strategy(parsing_strategy)
                         {}

        void enqueue_msg(std::string msg);
        void init_thread();
        void stop_thread();

        void wait_until_queue_empty();

        bool thread_active() { return is_thread_running; }

    private:
        std::function<void(std::string)> enqueue_to_log_writer_callback;
        std::function<void(std::string)> log_self_callback;
        std::function<bool()> log_writer_thread_active_callback;

        std::vector<std::string> parse_char_buffer(std::string buffer, ssize_t buffer_length);
        std::optional<std::string> generate_output_message(std::vector<std::string> parsed_buffer);
        void enqueue_log_writer(std::optional<std::string> msg);

        std::queue<std::string> msgs_to_parse;
        
        
        void run_thread();

        std::condition_variable thread_active_condition_var;
        std::mutex thread_active_mutex;
        std::thread writing_thread;
        bool is_thread_running = false;

        parser_strategy parsing_strategy;
    };

}

