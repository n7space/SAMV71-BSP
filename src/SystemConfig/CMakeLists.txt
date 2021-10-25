project(Samv71SystemConfig VERSION 1.0.0 LANGUAGES C)

add_library(Samv71SystemConfig INTERFACE)
target_sources(Samv71SystemConfig
    INTERFACE   SystemConfig.h)
