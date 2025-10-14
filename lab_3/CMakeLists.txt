cmake_minimum_required(VERSION 3.30)
project(lab_3)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(lab_3 main.cpp
        FileSystem.cpp
        FileSystem.h
)
