project(pyroxene_v2_backend)

add_library(socket_lib
    socket_comm.cpp
)

target_include_directories(socket_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})