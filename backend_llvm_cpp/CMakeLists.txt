cmake_minimum_required(VERSION 3.15)
project(pyroxene_v2_backend)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/socket_comm)

add_executable(driver
    src/main.cpp
)
target_link_libraries(driver
    socket_lib
)