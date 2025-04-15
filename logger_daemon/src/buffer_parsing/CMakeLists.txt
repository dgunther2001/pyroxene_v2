project(logger_daemon)

add_library(buffer_parsing_lib
    buffer_parser.cpp
)

target_include_directories(buffer_parsing_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(buffer_parsing_lib
    PUBLIC buffer_parsing_strategies_lib
)