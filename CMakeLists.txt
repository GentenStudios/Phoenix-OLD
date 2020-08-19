cmake_minimum_required(VERSION 3.8...3.16)

# Set the correct policies if CMake version is less than 3.12.
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(
    Phoenix
    VERSION 0.1
    LANGUAGES CXX)

# Use folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Update git submodules.
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(UPDATE_SUBMODULES "Update git submodules during build." ON)
    if(UPDATE_SUBMODULES)
        message(STATUS "Updating git submodules.")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMODULE_RESULT)
        if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMODULE_RESULT}, please checkout submodules.")
        endif()
    else()
        message(STATUS "Not updating")
    endif()
endif()

add_subdirectory(Phoenix)
