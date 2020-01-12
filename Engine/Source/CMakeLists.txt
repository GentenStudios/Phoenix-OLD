#add_subdirectory(Events)
#add_subdirectory(Graphics)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${currentDir}/Matrix4x4.cpp
	${currentDir}/Ray.cpp
	${currentDir}/ChunkRenderer.cpp
	${currentDir}/ShaderCompiler.cpp
	${currentDir}/Game.cpp
	${currentDir}/OpenGL32.cpp
	${currentDir}/Camera.cpp
	${currentDir}/Chunk.cpp
	${currentDir}/VoxelWorld.cpp
	${currentDir}/Mesh.cpp
	${currentDir}/BlocksTextureAtlas.cpp
	${currentDir}/BlocksRegistry.cpp
	${currentDir}/ContentLoader.cpp
	${currentDir}/Settings.cpp
	${currentDir}/ImGuiHelpers.cpp
	${currentDir}/Commander.cpp
	PARENT_SCOPE
)
