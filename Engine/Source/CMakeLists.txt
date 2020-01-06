add_subdirectory(Math)
add_subdirectory(Graphics)
add_subdirectory(Voxels)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${mathSources}
	${voxelSources}
	${graphicsSources}

	PARENT_SCOPE
)
