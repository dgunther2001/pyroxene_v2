cmake_minimum_required(VERSION 3.15)
project(logger_daemon)
set(CMAKE_CXX_STANDARD 20)

add_executable(driver
    src/logger.cpp
)
