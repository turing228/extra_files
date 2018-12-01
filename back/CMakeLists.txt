cmake_minimum_required(VERSION 2.8)

project(BACK_LIB)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold -Wall -std=c++11 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -D_GLIBCXX_DEBUG -g3")
endif ()

find_package(Qt5Widgets REQUIRED)

set(LIB clustering.cpp headers/clustering.h headers/consts.h)

add_library(BACK_LIB STATIC
        ${LIB})

qt5_use_modules(BACK_LIB Widgets)