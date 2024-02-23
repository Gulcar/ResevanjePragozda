cmake_minimum_required(VERSION 3.20)

project(ResevanjePragozda)

add_executable(${PROJECT_NAME}
    src/Main.cpp
    src/Risalnik.cpp
    src/Input.cpp
    src/Animacija.cpp
    src/Text.cpp
    src/Scene/Scena.cpp
    src/Scene/IgraScena.cpp
    src/Scene/TestScena.cpp
    src/Objekti/Gozd.cpp
    src/Objekti/Igralec.cpp
    src/Objekti/Pomocnik.cpp
    src/Objekti/TileMap.cpp
    src/Objekti/Zlobnez.cpp
    src/Objekti/MiniMap.cpp
    src/Ostalo.cpp
    libs/glad3.3/src/glad.c
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PRIVATE
    libs/glfw/include
    libs/glad3.3/include
    libs/glm
    libs/
)

add_subdirectory(libs/glfw)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
)

