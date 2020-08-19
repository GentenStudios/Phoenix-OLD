cmake_minimum_required(VERSION 3.8...3.16)

# Set the correct policies for the current CMake version.
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(Phoenix VERSION 0.1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(Phoenix)
