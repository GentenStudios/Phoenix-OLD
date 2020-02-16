add_subdirectory(Math)
add_subdirectory(Graphics)
add_subdirectory(Voxels)
add_subdirectory(GUI)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${mathSources}
	${graphicsSources}
	${voxelSources}
	${guiSources}

	${currentDir}/ContentLoader.cpp
	${currentDir}/Settings.cpp
	${currentDir}/ImGuiHelpers.cpp
	${currentDir}/UI.cpp
	${currentDir}/Commander.cpp

	${currentDir}/Actor.cpp
	${currentDir}/Player.cpp

	${currentDir}/Main.cpp


	PARENT_SCOPE
)
