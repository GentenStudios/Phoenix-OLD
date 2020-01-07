add_subdirectory(Math)
add_subdirectory(Graphics)
add_subdirectory(Events)
add_subdirectory(Voxels)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${voxelHeaders}
	${graphicsHeaders}
	${eventHeaders}

	${currentDir}/CoreIntrinsics.hpp
	${currentDir}/FileIO.hpp
	${currentDir}/EnumTools.hpp
	${currentDir}/Singleton.hpp

	${currentDir}/Player.hpp
	${currentDir}/ClientPlayer.hpp

	PARENT_SCOPE
)
