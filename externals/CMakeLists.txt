cmake_minimum_required(VERSION 3.21)

if(PNX_USE_BUNDLE_SDL2)
    add_subdirectory(SDL EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 REQUIRED)
endif()

set(ENABLE_PROGRAMS OFF CACHE BOOL "Build Mbed TLS programs." FORCE)
set(UNSAFE_BUILD ON CACHE BOOL "Allow unsafe builds. These builds ARE NOT SECURE." FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "Build Mbed TLS tests." FORCE)
add_subdirectory(boost)
add_subdirectory(mbedtls)

target_compile_options(mbedcrypto PRIVATE -Wno-everything)
target_compile_options(boost_container PRIVATE -Wno-aggressive-loop-optimizations)