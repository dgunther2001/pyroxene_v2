cmake_minimum_required(VERSION 3.15)
project(logger_daemon_pyroxene)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../lib")

add_subdirectory(pyroxene_logger_strategy)

find_package(logger_foundry REQUIRED)

add_executable(driver
    src/logger.cpp
)

target_link_libraries(driver
    PRIVATE logger_foundry::logger_foundry
    PRIVATE pyroxene_logger_strategy_lib
)
