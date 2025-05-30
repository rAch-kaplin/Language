cmake_minimum_required(VERSION 3.13)

project(do_project LANGUAGES CXX)

#**********************************************************************************************************************

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#**********************************************************************************************************************

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#**********************************************************************************************************************

set(DEBUG_FLAGS
    -D_DEBUG
    -ggdb3
    -O0
)

#**********************************************************************************************************************

set(COMMON_COMPILE_OPTIONS
    -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat
    -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts
    -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body
    -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness
    -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd
    -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls
    -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel
    -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods
    -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum
    -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast
    -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers
    -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector
    -Wstack-usage=8192 -Werror=vla
)

#**********************************************************************************************************************

set(SANITIZER_FLAGS
    -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
    -fcheck-new
    -fsized-deallocation
    -fstack-protector
    -fstrict-overflow
    -flto-odr-type-merging
    -fno-omit-frame-pointer
    -pie
    -fPIE
)

#**********************************************************************************************************************

add_compile_options(${DEBUG_FLAGS} ${COMMON_COMPILE_OPTIONS} ${SANITIZER_FLAGS})
add_link_options(${SANITIZER_FLAGS})

#**********************************************************************************************************************

add_subdirectory(libs/logger)
add_subdirectory(libs/arg_parser)
add_subdirectory(frontend)
add_subdirectory(backend)

