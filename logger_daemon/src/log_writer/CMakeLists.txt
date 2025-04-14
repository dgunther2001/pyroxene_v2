project(logger_daemon)

add_library(log_writer_lib
    log_writer.cpp
)

target_include_directories(log_writer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})