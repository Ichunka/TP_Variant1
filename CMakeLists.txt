cmake_minimum_required(VERSION 3.31)
project(TP_Variant1)

set(CMAKE_CXX_STANDARD 23)

add_executable(TP_Variant1 main.cpp
        type_traits.h
        storage.h)
