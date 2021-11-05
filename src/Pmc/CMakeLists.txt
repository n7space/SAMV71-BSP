project(Samv71Pmc VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Pmc STATIC)
target_sources(Samv71Pmc
    PRIVATE     Pmc.c
    PUBLIC      Pmc.h
                PmcPeripheralId.h
                PmcRegisters.h)
target_include_directories(Samv71Pmc
    PRIVATE     ..)
target_link_libraries(Samv71Pmc
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Pmc PROPERTIES OUTPUT_NAME "pmc")
add_library(SAMV71::Pmc ALIAS Samv71Pmc)
