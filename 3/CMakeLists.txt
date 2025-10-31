cmake_minimum_required(VERSION 3.30)
project(lab_3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# AddressSanitizer
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

# Ищем встроенным модулем (FindCurses.cmake)
find_package(Curses REQUIRED)

# Добавляем include-директории
include_directories(${CURSES_INCLUDE_DIRS})

# Ищем дополнительные библиотеки panel и menu вручную
find_library(PANEL_LIB panel PATHS /opt/homebrew/opt/ncurses/lib REQUIRED)
find_library(MENU_LIB menu PATHS /opt/homebrew/opt/ncurses/lib REQUIRED)

# Исполняемый файл
add_executable(
  lab_3
  main.cpp
  FSNode.h
  FSNode.cpp
  FileSystem.h
  FileSystem.cpp
  TUI.h
  TUI.cpp)

# Линкуем всё нужное
target_link_libraries(lab_3 PRIVATE ${CURSES_LIBRARIES} ${PANEL_LIB}
                                    ${MENU_LIB})

# Для отладки — вывести, что именно найдено
message(STATUS "CURSES_LIBRARIES = ${CURSES_LIBRARIES}")
message(STATUS "PANEL_LIB = ${PANEL_LIB}")
message(STATUS "MENU_LIB = ${MENU_LIB}")
