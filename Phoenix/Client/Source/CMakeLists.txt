add_subdirectory(Graphics)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${graphicsSources}

	${currentDir}/Client.cpp
	${currentDir}/ImGuiHelpers.cpp
	${currentDir}/SplashScreen.cpp
	${currentDir}/Game.cpp
	${currentDir}/DebugOverlay.cpp

	${currentDir}/Player.cpp

	${currentDir}/Main.cpp

	PARENT_SCOPE
)
