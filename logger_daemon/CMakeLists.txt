cmake_minimum_required(VERSION 3.15)
project(logger_daemon)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/buffer_parser_strategies)
add_subdirectory(src/buffer_parsing)
add_subdirectory(src/log_writer)
add_subdirectory(src/input_socket)
add_subdirectory(src/daemon_orchestrator)

add_executable(driver
    src/logger.cpp
)

target_link_libraries(driver
    PRIVATE daemon_orchestrator_lib
)
