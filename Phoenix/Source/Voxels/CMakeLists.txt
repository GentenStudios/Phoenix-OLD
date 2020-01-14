set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(voxelSources
	${currentDir}/BlockRegistry.cpp
	${currentDir}/TextureRegistry.cpp
	${currentDir}/Chunk.cpp

	PARENT_SCOPE
)
