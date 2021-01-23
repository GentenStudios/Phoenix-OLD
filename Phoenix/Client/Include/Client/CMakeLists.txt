add_subdirectory(Events)
add_subdirectory(Graphics)
add_subdirectory(Voxels)
add_subdirectory(Game)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/InputQueue.hpp

	${currentDir}/Audio.hpp
	${currentDir}/Client.hpp
	${currentDir}/SplashScreen.hpp
	${currentDir}/Crosshair.hpp
	${currentDir}/Game.hpp
	${currentDir}/EscapeMenu.hpp
	${currentDir}/HUD.hpp
	${currentDir}/InputMap.hpp
	${currentDir}/Network.hpp

	${currentDir}/DebugOverlay.hpp
	${currentDir}/GameTools.hpp
	${currentDir}/InventoryUI.hpp

	PARENT_SCOPE
	)
