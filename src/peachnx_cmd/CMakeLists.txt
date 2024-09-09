cmake_minimum_required(VERSION 3.21)

add_executable(pnx)
target_sources(pnx PRIVATE
    main.cpp
)
target_link_libraries(pnx PRIVATE pnx_lib)