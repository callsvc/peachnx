cmake_minimum_required(VERSION 3.21)
project(Peachnx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -fno-stack-protector -flto")
option(PNX_USE_QT6 "Enable the use of the GUI")
option(PNX_USE_BUNDLE_SDL2 "Enable the internal version of the SDL2 library")

find_package(OpenGL REQUIRED)

add_subdirectory(external)

if(PNX_USE_QT6)
    find_package(Qt6 REQUIRED COMPONENTS Widgets REQUIRED)
    qt_standard_project_setup()
endif()

add_subdirectory(src)