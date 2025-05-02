project(logger LANGUAGES CXX)

add_library(logger_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
)

target_sources(logger_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
)

target_include_directories(logger_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set_target_properties(logger_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
