add_subdirectory(Math)
add_subdirectory(Voxels)
add_subdirectory(Mods)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${mathSources}
	${voxelSources}
	${modSources}

	${currentDir}/ContentLoader.cpp
	${currentDir}/Settings.cpp
	${currentDir}/Logger.cpp
	${currentDir}/Commander.cpp

	PARENT_SCOPE
)
