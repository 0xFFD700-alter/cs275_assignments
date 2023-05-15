####################
# A simple CMake Project File for your reference
# Modify whatever you want
# You can also use other project frameworks
####################

cmake_minimum_required(VERSION 3.18)

project(stable-fluids-simulation)
set (CMAKE_CXX_STANDARD 20)

# It is recommended to use vcpkg to install these package
# visit https://github.com/Microsoft/vcpkg for more information
find_package(Eigen3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Add a interface library to link all dependency libraries
add_library(libdeps INTERFACE)
target_link_libraries(libdeps INTERFACE
    Eigen3::Eigen
    glfw
    glm::glm
    glad::glad
)

# Add the main library
add_library(libmain
    src/shader/shader.cpp
    src/render/fluid.cpp
    src/fluid/scene.cpp
    src/fluid/simulator.cpp)
target_include_directories(libmain
    PUBLIC include
)
target_link_libraries(libmain libdeps)

# The main opengl framework for fluid simulation and rendering
add_executable(main test/main.cpp)
target_link_libraries(main libmain)