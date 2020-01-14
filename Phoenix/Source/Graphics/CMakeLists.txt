set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(graphicsSources
	${currentDir}/Window.cpp
	${currentDir}/Camera.cpp

	${currentDir}/ShaderPipeline.cpp

	${currentDir}/ChunkMesher.cpp
	${currentDir}/ChunkRenderer.cpp

	PARENT_SCOPE
)
