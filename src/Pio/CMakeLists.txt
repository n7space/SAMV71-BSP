project(Samv71Pio VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Pio STATIC)
target_sources(Samv71Pio
    PRIVATE     Pio.c
    PUBLIC      Pio.h
                PioRegisters.h)
target_include_directories(Samv71Pio
    PUBLIC      ..)
target_link_libraries(Samv71Pio
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Pio PROPERTIES OUTPUT_NAME "pio")
add_library(SAMV71::Pio ALIAS Samv71Pio)
