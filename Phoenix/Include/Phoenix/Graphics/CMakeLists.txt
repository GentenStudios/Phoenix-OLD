set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(graphicsHeaders
	${currentDir}/OpenGLTools.hpp

	${currentDir}/Window.hpp
	${currentDir}/Camera.hpp

	${currentDir}/ShaderPipeline.hpp

	${currentDir}/ChunkMesher.hpp
	${currentDir}/ChunkRenderer.hpp

	PARENT_SCOPE
)
