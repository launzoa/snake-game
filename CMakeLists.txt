cmake_minimum_required(VERSION 3.16)
project(SNAKEGAME CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib REQUIRED)

add_executable(app
    main.cpp
    src/game.cpp
    src/algorithms.cpp
)

target_include_directories(app PRIVATE headers)
target_link_libraries(app PRIVATE raylib)