#include "buffer_parser.h"

namespace util_buffer_parser {
    std::string get_current_formatted_time() {
        auto now = std::chrono::system_clock::now();
        auto time_c = std::chrono::system_clock::to_time_t(now);
        std::tm time_tm = *std::localtime(&time_c);
        std::stringstream ss;
        ss << std::put_time(&time_tm, /*"%Y-%m-%d */"%H:%M:%S");
        return ss.str();
    }

    log_level get_error_level() {
        const char* err_level_env = std::getenv("ERROR_LEVEL");
        if (!err_level_env) {
            return log_level::ERROR;
        }
        return parse_err_level_str(err_level_env);
    }

    log_level parse_err_level_str(const std::string& msg) {
        if (msg == "TRACE") return log_level::TRACE;
        if (msg == "DEBUG") return log_level::DEBUG;
        if (msg == "INFO")  return log_level::INFO;
        if (msg == "WARN")  return log_level::WARN;
        if (msg == "ERROR") return log_level::ERROR;
        if (msg == "FATAL") return log_level::FATAL;
        
        std::cerr << "Invalid error level - defaulting to ERROR\n";
        return log_level::ERROR;
    }

    valid_log_fields get_log_field(const std::string& log_field) {
        static const std::unordered_map<std::string, valid_log_fields> log_fields = {
            {"LOG_LEVEL", valid_log_fields::LOG_LEVEL},
            {"LOG_TYPE", valid_log_fields::LOG_TYPE},
            {"COMPONENT", valid_log_fields::COMPONENT},
            {"LANGUAGE", valid_log_fields::LANGUAGE},
            {"MESSAGE", valid_log_fields::MESSAGE}
        };

        auto log_field_lookup = log_fields.find(log_field);
        if (log_field_lookup != log_fields.end()) {
            return log_field_lookup->second;
        }

        return valid_log_fields::INVALID;
    }
}

namespace buffer_parser { 
    void buffer_parser_obj::enqueue_msg(std::string msg) {
        {
            std::lock_guard<std::mutex> lock(thread_active_mutex);
            msgs_to_parse.push(msg);
        }
        thread_active_condition_var.notify_one();
    }

    void buffer_parser_obj::enqueue_log_writer(std::optional<std::string> msg) {
        if (msg) {
            enqueue_to_log_writer_callback(*msg);
        }
    }

    std::vector<std::string> buffer_parser_obj::parse_char_buffer(std::string buffer, ssize_t buffer_length) {
        bool err_level_checked = false;
        std::string aggregate;
        std::vector<std::string> parsed_buffer;
        parsed_buffer.reserve(5);
        for (size_t buffer_pos = 0; buffer_pos < buffer_length; buffer_pos++) {
            if (buffer[buffer_pos] == '|') {
                if (!err_level_checked) {
                    util_buffer_parser::log_level error_level = util_buffer_parser::get_error_level();
                    util_buffer_parser::log_level msg_level = util_buffer_parser::parse_err_level_str(aggregate);
                    
                    // if the passed message is less than the current error level, skip it
                    if (msg_level < error_level) {
                        return {};
                    }
                    err_level_checked = true;
                }


                parsed_buffer.push_back(aggregate);
                aggregate.clear();
            } else {
                aggregate += buffer[buffer_pos];
            }
        }

        // use the | pipe operator here
        // parse error level
        // parse the msg type
        // parse the stage
        // parse the language
        // parse the message

        return parsed_buffer;

    }

    std::optional<std::string> buffer_parser_obj::generate_output_message(std::vector<std::string> parsed_buffer) {
        if (parsed_buffer.size() != 5) {
            return std::nullopt;
        }
        std::string output_msg;
        output_msg += util_buffer_parser::get_current_formatted_time();
        output_msg += " [" + parsed_buffer.at(0) + "] ";
        output_msg += "[" + parsed_buffer.at(2) + "] ";
        output_msg += "[" + parsed_buffer.at(3) + "] ";
        output_msg += parsed_buffer.at(4);
        return output_msg;

    }

    void buffer_parser_obj::run_thread() {
        while (is_thread_running || !msgs_to_parse.empty()) {
            std::unique_lock<std::mutex> lock(thread_active_mutex);
            thread_active_condition_var.wait(lock, [this] { return !msgs_to_parse.empty() || !is_thread_running; });

            while (!msgs_to_parse.empty()) {
                auto current_message = msgs_to_parse.front(); 
                msgs_to_parse.pop();
                auto parsed_msg = parse_char_buffer(current_message, current_message.size());
                auto output_msg = generate_output_message(parsed_msg);
                enqueue_log_writer(output_msg);
            }
        }
        //thread_active_condition_var.notify_all();
    }

    void buffer_parser_obj::init_thread() {
        if (!log_writer_thread_active_callback()) {
            // ADD ERROR HANDLING
        }
        is_thread_running = true;
        writing_thread = std::thread(&buffer_parser_obj::run_thread, this);
    }

    void buffer_parser_obj::stop_thread() {
        {
            std::lock_guard<std::mutex> lock(thread_active_mutex);
            is_thread_running = false;
            thread_active_condition_var.notify_one();
        }

        if (writing_thread.joinable()) {
            writing_thread.join();
        }
    }

    void buffer_parser_obj::wait_until_queue_empty() {
        std::unique_lock<std::mutex> lock(thread_active_mutex);
        thread_active_condition_var.wait(lock, [this]() { 
            return msgs_to_parse.empty(); 
        });
    }
    
}