add_subdirectory(Math)
add_subdirectory(Voxels)
set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Tests
        ${Tests}

        ${currentDir}/Main.cpp

        PARENT_SCOPE
        )