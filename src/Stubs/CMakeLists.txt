project(Samv71Stubs VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Stubs STATIC)
target_sources(Samv71Stubs
    PRIVATE     Stubs.c
    PUBLIC      Stubs.h)
target_include_directories(Samv71Stubs
    PUBLIC      ..)
target_link_libraries(Samv71Stubs
    PRIVATE     common_build_options
                bsp_build_options
                SAMV71::Pio
                SAMV71::Pmc
                SAMV71::Uart)

set_target_properties(Samv71Stubs PROPERTIES OUTPUT_NAME "stubs")
add_library(SAMV71::Stubs ALIAS Samv71Stubs)
