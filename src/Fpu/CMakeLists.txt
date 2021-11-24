project(Samv71Fpu VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Fpu STATIC)
target_sources(Samv71Fpu
    PRIVATE     Fpu.c
    PUBLIC      Fpu.h
                FpuRegisters.h)
target_include_directories(Samv71Fpu
    PUBLIC      ..)
target_link_libraries(Samv71Fpu
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Fpu PROPERTIES OUTPUT_NAME "fpu")
add_library(SAMV71::Fpu ALIAS Samv71Fpu)
