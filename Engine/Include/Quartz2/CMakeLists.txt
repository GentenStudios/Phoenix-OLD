add_subdirectory(Math)
add_subdirectory(Events)
#add_subdirectory(Graphics)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${eventHeaders}

	${currentDir}/EnumTools.hpp

	${currentDir}/Math.hpp
	${currentDir}/MathUtils.hpp
	${currentDir}/Matrix4x4.hpp
	${currentDir}/Quartz.hpp
	${currentDir}/Ray.hpp
	${currentDir}/Rect.hpp
	${currentDir}/Vec2.hpp
	${currentDir}/Vec3.hpp
	${currentDir}/ChunkRenderer.hpp
	${currentDir}/ShaderCompiler.hpp
	${currentDir}/Game.hpp
	${currentDir}/OpenGL32.hpp
	${currentDir}/Camera.hpp
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

	PARENT_SCOPE
)
