cmake_minimum_required(VERSION 3.15)

project(UNO5_Linux)
add_executable(UNO5_Linux src/main.cpp src/glad/glad.c src/Shader.h src/stb_image.h src/Camera.h src/Mesh.h src/Model.h src/Terrain.h)

set(CMAKE_CXX_STANDARD 17)

set(OpenGlLinkers -lGLU -lGL ) # setting all the Glut libraries as one variable.
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(assimp REQUIRED)
set(LIBS ${ASSIMP_LIBRARY})
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(${GLFW_INCLUDE_DIRS})
target_link_libraries(UNO5_Linux ${GLFW_LIBRARIES})
target_link_libraries(UNO5_Linux ${GLFW_STATIC_LIBRARIES})
target_link_libraries(${PROJECT_NAME} assimp)
include_directories( ${OPENGL_INCLUDE_DIRS} "/home/raffael/Documents/Schule/19-20/ANDERE PROJEKTE/Opengl/UNO5_Linux/dependencies/include")
link_directories("/home/raffael/Documents/Schule/19-20/ANDERE PROJEKTE/Opengl/UNO5_Linux/dependencies/libary")

target_link_libraries(UNO5_Linux ${OPENGL_LIBRARIES} )

