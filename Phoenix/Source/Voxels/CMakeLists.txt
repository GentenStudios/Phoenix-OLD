set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(voxelSources
	${currentDir}/BlockRegistry.cpp
	${currentDir}/TextureRegistry.cpp
	${currentDir}/Chunk.cpp
	${currentDir}/ChunkManager.cpp

	PARENT_SCOPE
)
