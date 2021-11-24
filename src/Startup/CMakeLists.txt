project(Samv71Startup VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Startup STATIC)
target_sources(Samv71Startup
    PRIVATE     startup_samv71q21.c
    PUBLIC      startup_samv71q21.h)
target_include_directories(Samv71Startup
    PUBLIC      ..)
target_link_libraries(Samv71Startup
    PRIVATE     common_build_options
                bsp_build_options
                SAMV71::Nvic)

set_target_properties(Samv71Startup PROPERTIES OUTPUT_NAME "startup")
add_library(SAMV71::Startup ALIAS Samv71Startup)


