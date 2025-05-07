project(frontend LANGUAGES CXX)

file(GLOB_RECURSE FRONTEND_SOURCES
    "src/*.cpp"
    "${CMAKE_SOURCE_DIR}/graphviz/graph_dump.cpp"
)

add_executable(frontend ${FRONTEND_SOURCES} src/write_tree_file.cpp)

target_include_directories(frontend PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/DSL
)

target_link_libraries(frontend PRIVATE
    logger_lib
    arg_parser_lib
)

set_target_properties(frontend PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
