set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(graphicsHeaders
	${currentDir}/OpenGLTools.hpp
	${currentDir}/ImGuiExtensions.hpp

	${currentDir}/Window.hpp
	${currentDir}/Layer.hpp
	${currentDir}/LayerStack.hpp
	${currentDir}/ImGuiHelpers.hpp
	${currentDir}/ImGuiHelpers.inl
	${currentDir}/UI.hpp

	${currentDir}/ShaderPipeline.hpp
	${currentDir}/Camera.hpp

	${currentDir}/ChunkMesher.hpp
	${currentDir}/ChunkRenderer.hpp
	${currentDir}/ChunkView.hpp

	PARENT_SCOPE
)
