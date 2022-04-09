cmake_minimum_required(VERSION 3.21)
project(SimpleLanguage)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(SimpleLanguage
        lex.cpp
        lex.h
        parse.cpp
        parse.h
        prog2.cpp)
