cmake_minimum_required (VERSION 3.20)
project (final)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_subdirectory (external)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	common
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	${GLUT_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(final 
	main.cpp
	common/objloader.cpp
	common/objloader.hpp
	common/shader.cpp
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp
)

target_link_libraries(final
	${ALL_LIBS}
)