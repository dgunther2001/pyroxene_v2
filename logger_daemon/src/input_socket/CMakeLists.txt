project(logger_daemon)

add_library(input_socket_lib
    input_socket.cpp
)

target_include_directories(input_socket_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})