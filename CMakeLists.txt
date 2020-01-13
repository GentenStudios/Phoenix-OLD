cmake_minimum_required(VERSION 3.8)
project(Quartz2)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(LIBCXX_ENABLE_EXPERIMENTAL_LIBRARY ON)

add_subdirectory(Engine)
add_subdirectory(Sandbox)
