cmake_minimum_required(VERSION 3.10)
project(DSOPP_synthesis C)

set(CMAKE_C_STANDARD 99)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")

add_executable(DSOPP_synthesis test.c bool_plus.h bool_plus.c utils.h arraylist.c arraylist.h bool_utils.c bool_utils.h linkedlist.c linkedlist.h)

target_link_libraries(DSOPP_synthesis m)
