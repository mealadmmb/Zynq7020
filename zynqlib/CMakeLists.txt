cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled main.cpp ZynqLibrary/UART/xUarts.h ZynqLibrary/UART/xUarts.cpp ZynqLibrary/Misc/ZynqSingleton.h)
