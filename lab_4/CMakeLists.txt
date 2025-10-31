cmake_minimum_required(VERSION 3.30)
project(lab_4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# AddressSanitizer
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

# executable
add_executable(
  lab_4
  main.cpp
  Structs.h
  Structs.cpp
  MST.h
  MST.cpp
)
