add_subdirectory(GUI)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${Sources}

	${currentDir}/Window.cpp
	${currentDir}/LayerStack.cpp
	${currentDir}/Camera.cpp
	${currentDir}/ImGuiExtensions.cpp

	${currentDir}/ShaderPipeline.cpp

	${currentDir}/ChatBox.cpp

	${currentDir}/TexturePacker.cpp
	${currentDir}/ChunkMesher.cpp
	${currentDir}/ChunkRenderer.cpp
	${currentDir}/SelectionBoxRenderer.cpp
	${currentDir}/SkyboxRenderer.cpp

	PARENT_SCOPE
)
