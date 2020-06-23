add_subdirectory(Math)
add_subdirectory(Voxels)
add_subdirectory(CMS)
add_subdirectory(Serialization)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${voxelHeaders}
	${cmsHeaders}
	${serializationHeaders}

	${currentDir}/CoreIntrinsics.hpp
	${currentDir}/EnumTools.hpp
	${currentDir}/Singleton.hpp
	${currentDir}/FileIO.hpp
	${currentDir}/Logger.hpp

	${currentDir}/Settings.hpp
	${currentDir}/Commander.hpp
	${currentDir}/Actor.hpp
	${currentDir}/Position.hpp
	${currentDir}/Movement.hpp
	${currentDir}/Input.hpp

	PARENT_SCOPE
	)
