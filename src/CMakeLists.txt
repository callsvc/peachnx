cmake_minimum_required(VERSION 3.21)

add_library(pnx_lib STATIC)
include_directories(.)
target_sources(pnx_lib PUBLIC
    crypto/ticket.cpp
    crypto/ticket.h
    crypto/keysdb.cpp
    crypto/keysdb.h
    crypto/keys_types.cpp
    crypto/keys_types.h
    crypto/aes_storage.cpp
    crypto/aes_storage.h
    crypto/checksum.cpp
    crypto/checksum.h
    settings/configuration.cpp
    settings/configuration.h
    settings/options_type.h
    settings/options.h
    service/process.h
    core/application.cpp
    core/application.h
    core/assets_backing.cpp
    core/assets_backing.h
    core/games_list.cpp
    core/games_list.h
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
    disk/nca.cpp
    disk/nca.h
    disk/nca_filesystem_info.cpp
    disk/nca_filesystem_info.h
    disk/encrypted_ranged_file.cpp
    disk/encrypted_ranged_file.h
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
    meta/content_meta.cpp
    meta/content_meta.h
    kernel/kernel.h
    kernel/async.cpp
    kernel/async.h
    kernel/kprocess.cpp
    kernel/kprocess.h
    os/make_process.cpp
    os/make_process.h
    virtual_allocator.cpp
    virtual_allocator.h
    container.h
    generic.h
    types.cpp
    types.h
)

target_link_libraries(pnx_lib PUBLIC SDL2 OpenGL::GL Boost::context Boost::program_options mbedcrypto)

add_subdirectory(peachnx)
add_subdirectory(peachnx_cmd)
