cmake_minimum_required(VERSION 3.21)

add_library(pnx_lib STATIC)
include_directories(.)
target_sources(pnx_lib PUBLIC
    common/virtual_allocator.cpp
    common/virtual_allocator.h
    common/common_types.h
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
    service/am/am_types.h
    service/am/applet_manager.h
    kernel/kernel.cpp
    kernel/kernel.h
    kernel/async.cpp
    kernel/async.h
    types.h
)

target_link_libraries(pnx_lib PUBLIC SDL2 OpenGL::GL)

add_subdirectory(peachnx)
add_subdirectory(peachnx_cmd)
