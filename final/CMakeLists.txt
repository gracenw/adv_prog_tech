cmake_minimum_required(VERSION 3.20)

project(final)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

include_directories(common)
include_directories(glm)

add_executable(final
        main.cpp
        common/objloader.cpp
        common/objloader.hpp
)

add_library(glm STATIC IMPORTED)
target_link_libraries(final ${ALL_LIBS})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} )

target_link_libraries(final ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )