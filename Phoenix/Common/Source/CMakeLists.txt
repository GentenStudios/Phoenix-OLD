add_subdirectory(Math)
add_subdirectory(Voxels)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${mathSources}
	${voxelSources}

	${currentDir}/ContentLoader.cpp
	${currentDir}/Settings.cpp
	${currentDir}/Commander.cpp
	${currentDir}/Actor.cpp

	PARENT_SCOPE
)
