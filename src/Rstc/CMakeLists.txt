project(Samv71Rstc VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Rstc STATIC)
target_sources(Samv71Rstc
    PRIVATE     Rstc.c
    PUBLIC      Rstc.h
                RstcRegisters.h)
target_include_directories(Samv71Rstc
    PUBLIC      ..)
target_link_libraries(Samv71Rstc
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Rstc PROPERTIES OUTPUT_NAME "rstc")
add_library(SAMV71::Rstc ALIAS Samv71Rstc)
