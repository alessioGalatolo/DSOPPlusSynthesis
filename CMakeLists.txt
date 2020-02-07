cmake_minimum_required(VERSION 3.15)
project(SOP_synthesis C)

set(CMAKE_C_STANDARD 99)

add_executable(SOP_synthesis main.c SOP_plus.h SOP_plus.c utils.h list.c list.h bool_utils.c bool_utils.h)