cmake_minimum_required(VERSION 3.21)
project(C3_SimpleBashUtils_0 C)

set(CMAKE_C_STANDARD 11)

include_directories(src/cat)
#include_directories(src/grep)

add_executable(C3_SimpleBashUtils_0
        src/cat/cat.h
        src/cat/s21_cat.c
#        src/grep/s21_grep.c
#        src/grep/s21_grep.h
        )
