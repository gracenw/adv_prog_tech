cmake_minimum_required(VERSION 3.20)

project(final)

add_executable(final main.cpp)

find_package(OpenGL REQUIRED)

find_package(GLUT REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(final ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )