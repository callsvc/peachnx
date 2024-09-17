cmake_minimum_required(VERSION 3.21)

add_library(pnx_lib STATIC)
include_directories(.)
target_sources(pnx_lib PUBLIC
    crypto/ticket.cpp
    crypto/ticket.h
    crypto/keysdb.cpp
    crypto/keysdb.h
    crypto/keys_types.h
    settings/configuration.cpp
    settings/configuration.h
    settings/options_type.h
    settings/options.h
    service/process.cpp
    service/process.h
    core/application.cpp
    core/application.h
    core/assets_backing.cpp
    core/assets_backing.h
    loader/loader.cpp
    loader/loader.h
    loader/submission_package.cpp
    loader/submission_package.h
    loader/nsp.cpp
    loader/nsp.h
    disk/virtual_types.h
    disk/virtual_file.cpp
    disk/virtual_file.h
    disk/regular_file.cpp
    disk/regular_file.h
    disk/realfs.cpp
    disk/realfs.h
    disk/offset_file.cpp
    disk/offset_file.h
    disk/partition_filesystem.cpp
    disk/partition_filesystem.h
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
    kernel/kernel_types.h
    virtual_allocator.cpp
    virtual_allocator.h
    container.h
    types.cpp
    types.h
)

target_link_libraries(pnx_lib PUBLIC SDL2 OpenGL::GL Boost::context Boost::program_options)

add_subdirectory(peachnx)
add_subdirectory(peachnx_cmd)
