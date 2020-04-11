add_subdirectory(Math)
add_subdirectory(Voxels)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
		${mathSources}
		${voxelSources}

		${currentDir}/Actor.cpp

		${currentDir}/ContentLoader.cpp
		${currentDir}/Settings.cpp
		${currentDir}/Logger.cpp
		${currentDir}/Commander.cpp

		PARENT_SCOPE
		)
