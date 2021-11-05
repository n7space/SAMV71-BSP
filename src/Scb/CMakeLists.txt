project(Samv71Scb VERSION 1.0.0 LANGUAGES C)

add_library(Samv71Scb INTERFACE)
target_sources(Samv71Scb
    INTERFACE   Scb.h
                ScbRegisters.h)
