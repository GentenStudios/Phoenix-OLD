cmake_minimum_required(VERSION 3.12)

project(
    Phoenix
    VERSION 0.2
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
        message(STATUS "Git submodules set to manual update only, this can be changed with the UPDATE_SUBMODULES option.")
    endif()
endif()

add_subdirectory(Phoenix)
