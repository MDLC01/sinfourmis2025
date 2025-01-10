cmake_minimum_required(VERSION 3.28)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED True)

project(Fourmyriade)

add_library(Fourmyriade SHARED fourmis.cpp)

target_include_directories(Fourmyriade PRIVATE .)

string(APPEND CMAKE_CXX_FLAGS "-std=c++20 -Wall -Wextra -Werror -pedantic -Wpointer-arith -Wchar-subscripts -Wcomment -Wformat -Wwrite-strings -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -I../include/")
