add_subdirectory(Events)
add_subdirectory(Graphics)
add_subdirectory(Voxels)
add_subdirectory(Game)
add_subdirectory(UI)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/AudioRegistry.hpp
	${currentDir}/Client.hpp
	${currentDir}/Game.hpp
	${currentDir}/InputMap.hpp
	${currentDir}/InputQueue.hpp
	${currentDir}/Network.hpp

	PARENT_SCOPE
	)
