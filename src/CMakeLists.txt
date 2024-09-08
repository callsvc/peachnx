cmake_minimum_required(VERSION 3.21)

add_library(pnx_lib STATIC)
include_directories(.)
target_sources(pnx_lib PUBLIC
    settings/configuration.cpp
    settings/configuration.h
    settings/options_type.h
    settings/options.h
    core/process.cpp
    core/process.h
    core/assets_backing.cpp
    core/assets_backing.h
    loader/application.cpp
    loader/application.h
    loader/submission_package.cpp
    loader/submission_package.h
    disk/virtual_types.h
    disk/virtual_file.cpp
    disk/virtual_file.h
    disk/regular_file.cpp
    disk/regular_file.h
    disk/realfs.cpp
    disk/realfs.h
    surface/sdl_window.cpp
    surface/sdl_window.h
    surface/sdl_impl_gl.cpp
    surface/sdl_impl_gl.h
    types.h
)

target_link_libraries(pnx_lib PUBLIC SDL2::SDL2 OpenGL::GL)

if(USE_QT_UI)
    add_subdirectory(peachnx)
else()
    add_subdirectory(peachnx_cmd)
endif()