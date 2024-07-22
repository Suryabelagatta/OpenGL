cmake_minimum_required(VERSION 3.10)
project(OpenGLExample)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Explicitly include directories and link directories
include_directories(/opt/homebrew/Cellar/freeglut/3.4.0/include)
link_directories(/opt/homebrew/Cellar/freeglut/3.4.0/lib)

add_executable(OpenGLExample main.cpp)

# Link the libraries with correct framework syntax for macOS
target_link_libraries(OpenGLExample ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} "-framework OpenGL" "-framework GLUT")
