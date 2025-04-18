cmake_minimum_required(VERSION 3.15)
project(logger_daemon)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

install(TARGETS logger_foundry
    EXPORT logger_foundryTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT logger_foundryTargets
    FILE logger_foundryTargets.cmake
    NAMESPACE logger_foundry::
    DESTINATION lib
)

install(DIRECTORY include/logger_foundry
    DESTINATION include/logger_foundry
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/logger_foundryConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/logger_foundryConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/logger_foundryConfig.cmake"
    INSTALL_DESTINATION lib
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/logger_foundryConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/logger_foundryConfigVersion.cmake"
    DESTINATION lib
)

message(STATUS "Libraries found in ${CMAKE_INSTALL_PREFIX}/lib: ${ALL_LIBS}")
