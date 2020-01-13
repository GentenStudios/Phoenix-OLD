add_subdirectory(Math)
add_subdirectory(Graphics)
add_subdirectory(Voxels)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${mathSources}
	${graphicsSources}
	${voxelSources}

	${currentDir}/ContentLoader.cpp
	${currentDir}/Settings.cpp
	${currentDir}/ImGuiHelpers.cpp

	PARENT_SCOPE
)
