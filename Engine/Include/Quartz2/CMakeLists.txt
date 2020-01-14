add_subdirectory(Math)
add_subdirectory(Events)
add_subdirectory(Graphics)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${eventHeaders}
	${graphicsHeaders}

	${currentDir}/EnumTools.hpp

	${currentDir}/Quartz.hpp
	${currentDir}/ChunkRenderer.hpp
	${currentDir}/ShaderCompiler.hpp
	${currentDir}/Game.hpp
	${currentDir}/OpenGL32.hpp
	${currentDir}/Chunk.hpp
	${currentDir}/VoxelWorld.hpp
	${currentDir}/Mesh.hpp
	${currentDir}/Vertex.hpp
	${currentDir}/BlocksTextureAtlas.hpp
	${currentDir}/BlocksRegistry.hpp
	${currentDir}/Singleton.hpp
	${currentDir}/ImGuiHelpers.hpp
	${currentDir}/ContentLoader.hpp
	${currentDir}/Settings.hpp
	${currentDir}/Commander.hpp


	PARENT_SCOPE
)
