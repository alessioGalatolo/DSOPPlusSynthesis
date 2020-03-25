cmake_minimum_required(VERSION 3.10)
project(SOP_synthesis C)

set(CMAKE_C_STANDARD 99)

add_executable(SOP_synthesis main.c bool_plus.h bool_plus.c utils.h arraylist.c arraylist.h bool_utils.c bool_utils.h linkedlist.c linkedlist.h)

target_link_libraries(SOP_synthesis m)
