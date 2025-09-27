cmake_minimum_required(VERSION 3.30)
project(lab_1)

set(CMAKE_CXX_STANDARD 20)

add_executable(
  lab_1
  main.cpp
  Queue.h
  Queue.cpp
  Reader.h
  Reader.cpp
  Buffer.h
  Buffer.cpp)
