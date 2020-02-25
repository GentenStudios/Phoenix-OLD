add_subdirectory(Math)
add_subdirectory(Voxels)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${voxelHeaders}

	${currentDir}/CoreIntrinsics.hpp
	${currentDir}/EnumTools.hpp
	${currentDir}/Singleton.hpp
	${currentDir}/FileIO.hpp

	${currentDir}/ContentLoader.hpp
	${currentDir}/Settings.hpp
	${currentDir}/Commander.hpp
	${currentDir}/Actor.hpp

	PARENT_SCOPE
)
