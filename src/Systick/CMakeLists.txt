project(Samv71Systick VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Systick STATIC)
target_sources(Samv71Systick
    PRIVATE     Systick.c
    PUBLIC      Systick.h
                SystickRegisters.h)
target_include_directories(Samv71Systick
    PUBLIC      ..)
target_link_libraries(Samv71Systick
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Systick PROPERTIES OUTPUT_NAME "systick")
add_library(SAMV71::Systick ALIAS Samv71Systick)
