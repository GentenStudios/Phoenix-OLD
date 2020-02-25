set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(graphicsSources
	${currentDir}/ImGuiExtensions.cpp

	${currentDir}/Window.cpp
	${currentDir}/LayerStack.cpp
	${currentDir}/Camera.cpp

	${currentDir}/ShaderPipeline.cpp

	${currentDir}/UI.cpp
	${currentDir}/ChunkMesher.cpp
	${currentDir}/ChunkRenderer.cpp
	${currentDir}/ChunkView.cpp

	PARENT_SCOPE
)
