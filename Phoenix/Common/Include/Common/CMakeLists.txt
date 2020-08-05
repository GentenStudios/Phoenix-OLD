add_subdirectory(Math)
add_subdirectory(Voxels)
add_subdirectory(CMS)
add_subdirectory(Serialization)
add_subdirectory(Network)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${voxelHeaders}
	${cmsHeaders}
	${serializationHeaders}
	${networkHeaders}

	${currentDir}/CoreIntrinsics.hpp
	${currentDir}/EnumTools.hpp
	${currentDir}/Singleton.hpp
	${currentDir}/FileIO.hpp
	${currentDir}/Logger.hpp
	${currentDir}/Registry.hpp

	${currentDir}/Settings.hpp
	${currentDir}/Commander.hpp
	${currentDir}/Position.hpp
	${currentDir}/Movement.hpp
	${currentDir}/Input.hpp
	${currentDir}/Save.hpp
	${currentDir}/PlayerView.hpp
	${currentDir}/Actor.hpp

	PARENT_SCOPE
)
