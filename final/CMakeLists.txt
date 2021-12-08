cmake_minimum_required(VERSION 3.20)

project(final)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

include_directories(
        glm
        utils
        glew/include
)

add_executable(
        final
        main.cpp
        glew/include/GL/glxew.h
        glew/include/GL/glew.h
        glew/src/glew.c
        utils/objloader.hpp
        utils/objloader.cpp
)

#add_subdirectory(glew/build/cmake)
#link_directories(${CMAKE_SOURCE_DIR}/glew)

#add_library(glew STATIC SHARED)
add_library(glm STATIC IMPORTED)

target_link_libraries(final ${ALL_LIBS})

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
        ${X11_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIRS} 
        ${GLUT_INCLUDE_DIRS} 
)

target_link_libraries(
        final 
        ${X11_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY} 
)
