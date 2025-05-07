project(logger LANGUAGES CXX)

file(GLOB_RECURSE LOGGER_SOURCES "*.cpp")

add_library(logger_lib STATIC
    logger.cpp
    logger.h
)

target_sources(logger_lib PRIVATE
    logger.h
)

target_include_directories(logger_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(logger_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
