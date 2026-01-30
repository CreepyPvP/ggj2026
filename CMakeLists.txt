cmake_minimum_required(VERSION 3.25)

project(
    ggj2026
)

set(ROOT_DIR ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME}
	code/main.cpp
    code/scene.cpp
    code/game.cpp
)

set(RAYLIB_DIR raylib)
add_subdirectory(${RAYLIB_DIR})

target_link_libraries(${PROJECT_NAME} 
    raylib
)
