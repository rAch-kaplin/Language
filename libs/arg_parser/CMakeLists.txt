project(arg_parser LANGUAGES CXX)

add_library(arg_parser_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/arg_parser.cpp
)

target_sources(arg_parser_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/arg_parser.h
)

target_include_directories(arg_parser_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set_target_properties(arg_parser_lib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
