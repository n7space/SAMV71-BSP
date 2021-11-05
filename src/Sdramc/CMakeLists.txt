project(Samv71Sdramc VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Sdramc STATIC)
target_sources(Samv71Sdramc
    PRIVATE     Sdramc.c
    PUBLIC      Sdramc.h
                SdramcRegisters.h)
target_include_directories(Samv71Sdramc
    PRIVATE     ..)
target_link_libraries(Samv71Sdramc
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Sdramc PROPERTIES OUTPUT_NAME "sdramc")
add_library(SAMV71::Sdramc ALIAS Samv71Sdramc)
