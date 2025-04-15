project(logger_daemon)

add_library(daemon_orchestrator_lib
    daemon_orchestrator.cpp
)

target_include_directories(daemon_orchestrator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(daemon_orchestrator_lib
    PUBLIC log_writer_lib
    PUBLIC buffer_parsing_lib
    PUBLIC input_socket_lib
)