cmake_minimum_required(VERSION 3.21)

qt_add_executable(peachnx
    application.cpp
    main_window.cpp
    main_window.h

    game_item_widget.cpp
    game_item_widget.h
)
target_link_libraries(peachnx PRIVATE Qt6::Widgets pnx_lib)