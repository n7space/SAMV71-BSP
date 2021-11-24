project(Samv71Nvic VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Nvic STATIC)
target_sources(Samv71Nvic
    PRIVATE     Nvic.c
    PUBLIC      Nvic.h
                NvicRegisters.h
                NvicVectorTable.h)
target_include_directories(Samv71Nvic
    PUBLIC      ..)
target_link_libraries(Samv71Nvic
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Nvic PROPERTIES OUTPUT_NAME "nvic")
add_library(SAMV71::Nvic ALIAS Samv71Nvic)
