add_subdirectory(Math)
add_subdirectory(Voxels)
add_subdirectory(Mods)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${voxelHeaders}
	${modHeaders}

	${currentDir}/CoreIntrinsics.hpp
	${currentDir}/EnumTools.hpp
	${currentDir}/Singleton.hpp
	${currentDir}/FileIO.hpp
	${currentDir}/Logger.hpp

	${currentDir}/ContentLoader.hpp
	${currentDir}/Settings.hpp
	${currentDir}/Commander.hpp
	${currentDir}/Actor.hpp

	PARENT_SCOPE
)
