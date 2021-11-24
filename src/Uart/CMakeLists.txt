project(Samv71Uart VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Uart STATIC)
target_sources(Samv71Uart
    PRIVATE     Uart.c
    PUBLIC      Uart.h
                UartRegisters.h)
target_include_directories(Samv71Uart
    PUBLIC      ..)
target_link_libraries(Samv71Uart
    PRIVATE     common_build_options
                bsp_build_options)

set_target_properties(Samv71Uart PROPERTIES OUTPUT_NAME "uart")
add_library(SAMV71::Uart ALIAS Samv71Uart)
