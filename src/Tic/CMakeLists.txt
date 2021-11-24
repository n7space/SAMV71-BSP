project(Samv71Tic VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Tic STATIC)
target_sources(Samv71Tic
    PRIVATE     Tic.c
    PUBLIC      Tic.h
                TicRegisters.h)
target_include_directories(Samv71Tic
    PUBLIC      ..)
target_link_libraries(Samv71Tic
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Tic PROPERTIES OUTPUT_NAME "tic")
add_library(SAMV71::Tic ALIAS Samv71Tic)
