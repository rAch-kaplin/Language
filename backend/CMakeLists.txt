project(backend LANGUAGES CXX)

file(GLOB_RECURSE BACKEND_SOURCES
    "src/*.cpp"
    "${CMAKE_SOURCE_DIR}/frontend/src/lexical_analysis.cpp"
    "${CMAKE_SOURCE_DIR}/frontend/src/tree_func.cpp"
    "${CMAKE_SOURCE_DIR}/frontend/src/file_read.cpp"
    "${CMAKE_SOURCE_DIR}/frontend/src/syntax_analysis.cpp"
)
add_executable(backend ${BACKEND_SOURCES})

# target_include_directories(backend PRIVATE
#     include
#     ${CMAKE_SOURCE_DIR}/common
#     ${CMAKE_SOURCE_DIR}/DSL
# )

target_include_directories(backend PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/DSL
    ${CMAKE_SOURCE_DIR}/frontend/include
    ${CMAKE_SOURCE_DIR}/frontend/src
)

target_link_libraries(backend PRIVATE
    logger_lib
    arg_parser_lib
)

set_target_properties(backend PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
