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
    loader/nca.cpp
    loader/nca.h
    loader/content_archive.cpp
    loader/content_archive.h
    loader/nca_mount.cpp
    loader/nca_mount.h
    loader/content_meta.cpp
    loader/content_meta.h
    sys_fs/virtual_types.h
    sys_fs/virtual_file.cpp
    sys_fs/virtual_file.h
    sys_fs/regular_file.cpp
    sys_fs/regular_file.h
    sys_fs/realfs.cpp
    sys_fs/realfs.h
    sys_fs/encrypted_ranged_file.cpp
    sys_fs/encrypted_ranged_file.h
    sys_fs/offset_file.cpp
    sys_fs/offset_file.h
    sys_fs/partition_filesystem.cpp
    sys_fs/partition_filesystem.h
    surface/sdl_window.cpp
    surface/sdl_window.h
    surface/sdl_impl_gl.cpp
    surface/sdl_impl_gl.h
    service/am/am_types.h
    service/am/applet_manager.h
    service/make_process.cpp
    service/make_process.h
    kernel/kernel.h
    kernel/async.cpp
    kernel/async.h
    kernel/kprocess.cpp
    kernel/kprocess.h
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
