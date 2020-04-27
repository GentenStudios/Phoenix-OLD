add_subdirectory(Math)
add_subdirectory(Voxels)
add_subdirectory(CMS)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${mathSources}
	${voxelSources}
	${cmsSources}

	${currentDir}/Settings.cpp
	${currentDir}/Logger.cpp
	${currentDir}/Commander.cpp

	PARENT_SCOPE
)
