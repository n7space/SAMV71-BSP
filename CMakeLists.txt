cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(SAMV71-BSP VERSION 1.0.0 LANGUAGES C)

set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/samv71bsp)

add_subdirectory(src)
