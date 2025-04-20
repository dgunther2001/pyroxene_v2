#include <iostream>

#include <logger_foundry/logger_foundry.h>
#include "pyroxene_logger_parser_strategy.h"
#include "pyroxene_shutdown_strategy.h"
#include "test_socket.h"

int main() {
    /*
    {"LOG_LEVEL", valid_log_fields::LOG_LEVEL},
    {"LOG_TYPE", valid_log_fields::LOG_TYPE},
    {"COMPONENT", valid_log_fields::COMPONENT},
    {"LANGUAGE", valid_log_fields::LANGUAGE},
    {"MESSAGE", valid_log_fields::MESSAGE}
    */
    
    const char* msg  = "LOG_LEVEL=DEBUG|LOG_TYPE=LogInfo|COMPONENT=Logger|LANGUAGE=C++|MESSAGE=Logger Invoked|";
    const char* msg2 = "LOG_LEVEL=WARN|LOG_TYPE=LogInfo|COMPONENT=Logger|LANGUAGE=C++|MESSAGE=Logger Invoked|";
    const char* msg3 = "LOG_TYPE=LogInfo|COMPONENT=Logger|LANGUAGE=C++|MESSAGE=Logger Invoked|";

    //logger_foundry::logger_daemon orchestrator(std::getenv("PYROXENE_LOG_PATH"), std::getenv("PYROXENE_LOG_SOCKET_PATH"), &pyroxene_default_parser::pyroxene_default_parser);

    
    logger_foundry::logger_daemon orchestrator = logger_foundry::logger_daemon_builder()
        .set_log_path(std::getenv("PYROXENE_LOG_PATH"))
        .add_unix_socket(std::getenv("PYROXENE_LOG_SOCKET_PATH"), 20)
        .set_parser_strategy(&pyroxene_default_parser::pyroxene_default_parser)
        .set_kill_strategy([] {
            pyroxene_shutdown_strategy::monitor_feeding_processes();
        })
        .build();

    
    orchestrator.log_direct("LOG_LEVEL=DEBUG|LOG_TYPE=LogInfo|COMPONENT=Logger Orchestrator|LANGUAGE=C++|MESSAGE=Orchestrator Initializing Threads|");
    
    for (int i = 0; i < 2; i ++) {
        write_test_socket::write_dummy_log_message(msg);
        write_test_socket::write_dummy_log_message(msg2);
        write_test_socket::write_dummy_log_message(msg3);
    }
    
    //std::thread monitor_thread(monitor_feeding_processes);
    //monitor_thread.join();

    return 0;
    
}