project(logger_daemon)

add_library(input_socket_lib
    input_socket.cpp
)

target_include_directories(input_socket_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(input_socket_lib
    PUBLIC buffer_parsing_lib
)