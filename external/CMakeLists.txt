cmake_minimum_required(VERSION 3.21)

if(PNX_USE_BUNDLE_SDL2)
    add_subdirectory(SDL EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 REQUIRED)
endif()
add_subdirectory(boost)