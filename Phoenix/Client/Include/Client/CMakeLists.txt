add_subdirectory(Events)
add_subdirectory(Graphics)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${eventHeaders}
	${graphicsHeaders}

	${currentDir}/Client.hpp
	${currentDir}/SplashScreen.hpp
	${currentDir}/Crosshair.hpp
	${currentDir}/Game.hpp
	${currentDir}/DebugOverlay.hpp

	${currentDir}/Player.hpp

	PARENT_SCOPE
)
