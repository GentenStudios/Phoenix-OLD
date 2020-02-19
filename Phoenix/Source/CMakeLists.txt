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
	${currentDir}/UI.cpp
	${currentDir}/Commander.cpp

	${currentDir}/Actor.cpp
	${currentDir}/Player.cpp

	${currentDir}/Phoenix.cpp
	${currentDir}/SplashScreen.cpp
	${currentDir}/Game.cpp

	${currentDir}/Main.cpp


	PARENT_SCOPE
)
