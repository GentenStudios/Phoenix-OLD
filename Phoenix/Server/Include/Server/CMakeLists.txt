add_subdirectory(Voxels)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${voxelHeaders}

	${currentDir}/Server.hpp
	${currentDir}/Iris.hpp
	${currentDir}/Game.hpp
	${currentDir}/Commander.hpp

	PARENT_SCOPE
)
