cmake_minimum_required(VERSION 3.12)

project(
    Phoenix
    VERSION 0.2
    LANGUAGES CXX)

# Fail if running on MacOSX and the version is too low.
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_SYSTEM_VERSION VERSION_LESS "10.15")
    message(FATAL_ERROR "MAC OS version ${CMAKE_SYSTEM_VERSION}, std::filesystem is only available on MACOS 10.15 or newer.")
endif()

# Use folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(Phoenix)
