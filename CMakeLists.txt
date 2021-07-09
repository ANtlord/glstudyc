cmake_minimum_required(VERSION 3.5)

project(glstudyc LANGUAGES C)

add_executable(glstudyc main.c glad.c errors.c)
include_directories(./)
#find_package(OpenGL REQUIRED)
#find_package(GLFW REQUIRED)
target_link_libraries(glstudyc glfw OpenGL GLEW GLU dl)
#target_include_directories(glstudyc ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLU_INCLUDE_DIR})
#target_include_directories(glstudyc ./)
