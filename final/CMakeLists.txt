cmake_minimum_required(VERSION 3.20)

project(final)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

include_directories(glm)

add_executable(final
        main.cpp
)

add_library(glm STATIC IMPORTED)
target_link_libraries(final ${ALL_LIBS})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} )

target_link_libraries(final ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )