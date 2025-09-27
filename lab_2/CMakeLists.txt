cmake_minimum_required(VERSION 3.30)
project(
  lab_2
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(
        lab_2
        main.cpp
        Reader.h
        Reader.cpp
        Stack.h
        VarMapper.h
        VarMapper.cpp
        ExprParser.h
        ExprParser.cpp
)
