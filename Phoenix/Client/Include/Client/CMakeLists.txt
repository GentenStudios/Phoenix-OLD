add_subdirectory(Events)
add_subdirectory(Graphics)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${eventHeaders}
	${graphicsHeaders}

	${currentDir}/InputMap.hpp

	${currentDir}/Player.hpp

	${currentDir}/Client.hpp
	${currentDir}/SplashScreen.hpp
	${currentDir}/Crosshair.hpp
	${currentDir}/Game.hpp
	${currentDir}/EscapeMenu.hpp

	${currentDir}/DebugOverlay.hpp
	${currentDir}/GameTools.hpp

	PARENT_SCOPE
)
