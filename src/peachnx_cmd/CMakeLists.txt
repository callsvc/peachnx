cmake_minimum_required(VERSION 3.21)

add_executable(peachnx)
target_sources(peachnx PRIVATE
    main.cpp
)
target_link_libraries(peachnx PRIVATE pnx_lib)