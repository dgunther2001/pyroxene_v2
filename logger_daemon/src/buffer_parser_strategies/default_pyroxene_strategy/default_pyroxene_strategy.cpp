#include "default_pyroxene_strategy.h"

// internal needs
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unordered_map>


enum log_level {
    TRACE = 1,
    DEBUG = 10,
    INFO = 20,
    WARN = 30,
    ERROR = 40,
    FATAL = 50
};

enum valid_log_fields {
    LOG_LEVEL=1,
    LOG_TYPE=10,
    COMPONENT=20,
    LANGUAGE=30,
    MESSAGE=40,
    INVALID=100
};

std::string get_current_formatted_time() {
    auto now = std::chrono::system_clock::now();
    auto time_c = std::chrono::system_clock::to_time_t(now);
    std::tm time_tm = *std::localtime(&time_c);
    std::stringstream ss;
    ss << std::put_time(&time_tm, /*"%Y-%m-%d */"%H:%M:%S");
    return ss.str();
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

log_level get_error_level() {
    const char* err_level_env = std::getenv("ERROR_LEVEL");
    if (!err_level_env) {
        return log_level::ERROR;
    }
    return parse_err_level_str(err_level_env);
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







std::vector<std::string> parse_char_buffer(std::string buffer) {
    bool err_level_checked = false;
    std::string aggregate;
    std::vector<std::string> parsed_buffer;
    parsed_buffer.reserve(5);
    for (size_t buffer_pos = 0; buffer_pos < buffer.size(); buffer_pos++) {
        if (buffer[buffer_pos] == '|') {
            if (!err_level_checked) {
                log_level error_level = get_error_level();
                log_level msg_level = parse_err_level_str(aggregate);
                
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

    return parsed_buffer;

}

std::optional<std::string> generate_output_message(std::vector<std::string> parsed_buffer) {
    if (parsed_buffer.size() != 5) {
        return std::nullopt;
    }
    std::string output_msg;
    output_msg += get_current_formatted_time();
    output_msg += " [" + parsed_buffer.at(0) + "] ";
    output_msg += "[" + parsed_buffer.at(2) + "] ";
    output_msg += "[" + parsed_buffer.at(3) + "] ";
    output_msg += parsed_buffer.at(4);
    return output_msg;

}

namespace pyroxene_default_parser {
    std::optional<std::string> pyroxene_default_parser(const std::string& buffer) {
        return generate_output_message(parse_char_buffer(buffer));
    }
}