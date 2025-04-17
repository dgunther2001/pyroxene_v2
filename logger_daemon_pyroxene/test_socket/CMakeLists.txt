add_library(pyroxene_logger_socket_test_lib
    test_socket.cpp
)

target_include_directories(pyroxene_logger_socket_test_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})