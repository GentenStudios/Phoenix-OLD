add_subdirectory(Math)
add_subdirectory(Voxels)
add_subdirectory(CMS)
add_subdirectory(Network)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${mathSources}
	${voxelSources}
	${cmsSources}
	${networkSources}

	${currentDir}/Settings.cpp
	${currentDir}/Logger.cpp
	${currentDir}/Commander.cpp

	PARENT_SCOPE
)
