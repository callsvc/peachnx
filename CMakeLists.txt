cmake_minimum_required(VERSION 3.21)
project(Peachnx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2)

option(USE_QT_UI "Enable the use of the GUI")
if(USE_QT_UI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    qt_standard_project_setup()
endif()

add_subdirectory(src)