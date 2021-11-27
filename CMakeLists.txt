cmake_minimum_required(VERSION 3.20)
project(DSA_Asg3)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(DSA_Asg3
        error.h
        main.cpp
        main.h
        SymbolTable.cpp
        SymbolTable.h
        test.cpp)
