add_subdirectory(Graphics)
add_subdirectory(Game)
add_subdirectory(Voxels)
add_subdirectory(UI)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${Sources}

	${currentDir}/AudioRegistry.cpp
	${currentDir}/Client.cpp
	${currentDir}/Game.cpp
	${currentDir}/InputMap.cpp
	${currentDir}/InputQueue.cpp
	${currentDir}/Network.cpp

	${currentDir}/Main.cpp

	PARENT_SCOPE
	)
