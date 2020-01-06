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
	${currentDir}/EnumTools.hpp
	${currentDir}/Singleton.hpp

	PARENT_SCOPE
)
