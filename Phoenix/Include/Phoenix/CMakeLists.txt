add_subdirectory(Math)
add_subdirectory(Events)
add_subdirectory(Graphics)
add_subdirectory(Voxels)
add_subdirectory(GUI)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${eventHeaders}
	${graphicsHeaders}
	${voxelHeaders}
	${guiHeaders}

	${currentDir}/EnumTools.hpp
	${currentDir}/Singleton.hpp
	${currentDir}/FileIO.hpp

	${currentDir}/ImGuiHelpers.hpp
	${currentDir}/ContentLoader.hpp
	${currentDir}/Settings.hpp
	${currentDir}/UI.hpp
	${currentDir}/Commander.hpp

	${currentDir}/Actor.hpp
	${currentDir}/Player.hpp

	PARENT_SCOPE
)
