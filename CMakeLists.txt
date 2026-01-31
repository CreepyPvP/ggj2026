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
    code/entity.cpp
    code/player.cpp
	code/treasure.cpp
	code/level_selection.cpp

    external/LDtkLoader/DataTypes.cpp
    external/LDtkLoader/Entity.cpp
    external/LDtkLoader/EntityDef.cpp
    external/LDtkLoader/Enum.cpp
    external/LDtkLoader/FieldsContainer.cpp
    external/LDtkLoader/json.hpp
    external/LDtkLoader/Layer.cpp
    external/LDtkLoader/LayerDef.cpp
    external/LDtkLoader/Level.cpp
    external/LDtkLoader/Project.cpp
    external/LDtkLoader/TagsContainer.cpp
    external/LDtkLoader/Tile.cpp
    external/LDtkLoader/Tileset.cpp
    external/LDtkLoader/Utils.cpp
    external/LDtkLoader/World.cpp
		code/treasure.cpp
		code/treasure.h
		code/guard.cpp
		code/guard.h
)

set(RAYLIB_DIR raylib)
add_subdirectory(${RAYLIB_DIR})

target_include_directories(ggj2026 PUBLIC external)

target_link_libraries(${PROJECT_NAME} 
    raylib
)
