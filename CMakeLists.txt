cmake_minimum_required(VERSION 3.16)
project(snake-game C CXX)

set(CMAKE_CXX_STANDARD 17)

# Lista comum dos arquivos fonte do projeto
set(SOURCES
    main.cpp
    src/game.cpp
    src/algorithms.cpp
)

if (EMSCRIPTEN OR PLATFORM STREQUAL "Web")
    set(PLATFORM "Web" CACHE STRING "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(raylib)

    # Gera index.html (+ index.js, index.wasm, index.data)
    add_executable(index ${SOURCES})
    target_include_directories(index PRIVATE headers)
    target_link_libraries(index PRIVATE raylib)
    target_compile_definitions(index PRIVATE PLATFORM_WEB)

    set_target_properties(index PROPERTIES 
        SUFFIX ".html" 
        LINK_FLAGS "-s USE_GLFW=3 -s ASYNCIFY --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
    )

else()
    # Build Local (Linux / Desktop)
    find_package(raylib REQUIRED)
    add_executable(app ${SOURCES})
    target_include_directories(app PRIVATE headers)
    target_link_libraries(app PRIVATE raylib)
endif()