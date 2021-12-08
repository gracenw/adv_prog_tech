cmake_minimum_required(VERSION 3.20)

project(final)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(
        final
        main.cpp
        glew/include/GL/glxew.h
        glew/include/GL/glew.h
        glew/src/glew.c
)

add_library(glm STATIC IMPORTED)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
        glm
        glew/include
        ${X11_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIRS} 
        ${GLUT_INCLUDE_DIRS} 
)

target_link_libraries(
        final 
        ${ALL_LIBS}
        ${X11_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY} 
)
