project(Samv71Mcan VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Mcan STATIC)
target_sources(Samv71Mcan
    PRIVATE     Mcan.c
    PUBLIC      Mcan.h
                McanRegisters.h)
target_include_directories(Samv71Mcan
    PUBLIC      ..)
target_link_libraries(Samv71Mcan
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Mcan PROPERTIES OUTPUT_NAME "mcu")
add_library(SAMV71::Mcan ALIAS Samv71Mcan)
