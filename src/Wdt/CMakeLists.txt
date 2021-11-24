project(Samv71Wdt VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Wdt STATIC)
target_sources(Samv71Wdt
    PRIVATE     Wdt.c
    PUBLIC      Wdt.h
                WdtRegisters.h)
target_include_directories(Samv71Wdt
    PUBLIC      ..)
target_link_libraries(Samv71Wdt
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Wdt PROPERTIES OUTPUT_NAME "wdt")
add_library(SAMV71::Wdt ALIAS Samv71Wdt)
