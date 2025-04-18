#include "pyroxene_logger_parser_strategy.h"

#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace pyroxene_parser_strategy::internal {
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

    std::vector<std::pair<pyroxene_parser_strategy::internal::valid_log_fields, std::string>> parse_char_buffer(std::string buffer) {
        bool err_level_checked = false;
        std::string aggregate;
        pyroxene_parser_strategy::internal::valid_log_fields log_field;
        std::string log_field_string;
        std::string value_string;

        std::vector<std::pair<pyroxene_parser_strategy::internal::valid_log_fields, std::string>> parsed_buffer;

        for (char character : buffer) {
            if (character == '|') {
                auto equals_delimiter = aggregate.find('=');
                if (equals_delimiter != std::string::npos) {
                    log_field_string = aggregate.substr(0, equals_delimiter);
                    value_string = aggregate.substr(equals_delimiter + 1);
                    log_field = get_log_field(log_field_string);

                    if (log_field != pyroxene_parser_strategy::internal::valid_log_fields::INVALID) {
                        parsed_buffer.emplace_back(log_field, std::move(value_string));
                    }
                }

                aggregate.clear();
            } else {
                aggregate += character;
            }
        }
    
                // check for an equals sign
                // if there is none, then don't push back
                // if it is valid, split the aggregate by "="
                // check if it's a valid field => valid_log_fields current_field = get_log_field(split_string.first)
                // if it's not INVALID, then push the aggregate, with the second part of the string (after the equals) as well as the log field to the aggregate
    
        return parsed_buffer;
    
    }
    
    std::optional<std::string> generate_output_message(std::vector<std::pair<pyroxene_parser_strategy::internal::valid_log_fields, std::string>> parsed_buffer) {
    
        if (parsed_buffer.size() == 0) {
            return std::nullopt;
        }

        std::string output_msg;
        std::string current_time = get_current_formatted_time() + " ";



        bool contains_log_level = false;

        for (auto const& message_component : parsed_buffer) {
            switch (message_component.first) {
                case pyroxene_parser_strategy::internal::valid_log_fields::LOG_LEVEL: {
                    pyroxene_parser_strategy::internal::log_level error_level = get_error_level();
                    pyroxene_parser_strategy::internal::log_level msg_level = pyroxene_parser_strategy::internal::parse_err_level_str(message_component.second);
                    if (msg_level < error_level) {
                        return {};
                    }

                    output_msg += " [" + message_component.second + "] ";
                    contains_log_level = true;
                    break;
                }
                /*
                case pyroxene_parser_strategy::internal::valid_log_fields::LOG_TYPE:
                    output_msg += " [" + message_component.second + "] ";
                    break;
                */
                case pyroxene_parser_strategy::internal::valid_log_fields::COMPONENT:
                    output_msg += " [" + message_component.second + "] ";
                    break;
                case pyroxene_parser_strategy::internal::valid_log_fields::LANGUAGE:
                    output_msg +=" [" + message_component.second + "] ";
                    break;
                case pyroxene_parser_strategy::internal::valid_log_fields::MESSAGE:
                    output_msg += " " + message_component.second + " ";
                    break;
                default:
                    break;
            }
        }

        if (!contains_log_level) {
            if (log_level::ERROR < get_error_level()) {
                return {};
            }

            output_msg.insert(0, " [ERROR]");
        }

        output_msg.insert(0, current_time);

        output_msg += "\n";
        
        return output_msg;
    }
}


namespace pyroxene_default_parser {
    std::optional<std::string> pyroxene_default_parser(const std::string& buffer) {
        return pyroxene_parser_strategy::internal::generate_output_message(pyroxene_parser_strategy::internal::parse_char_buffer(buffer));
    }
}