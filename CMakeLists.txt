cmake_minimum_required(VERSION 3.20)

project(ResevanjePragozda)

add_executable(${PROJECT_NAME}
    src/Main.cpp
    libs/glad3.3/src/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    libs/glfw/include
    libs/glad3.3/include
)

add_subdirectory(libs/glfw)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
)

