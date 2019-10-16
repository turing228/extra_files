cmake_minimum_required(VERSION 3.1)

project(extra_files_finder)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

add_executable(extra_files_finder main.cpp mainwindow.h mainwindow.cpp deletedialog.h deletedialog.cpp back/headers/clusterWorker.h back/clusterWorker.cpp)
qt5_use_modules(extra_files_finder Widgets)

add_subdirectory(back)
include_directories(back)

#add_subdirectory(test)