add_subdirectory(Math)
add_subdirectory(Events)
add_subdirectory(Graphics)
add_subdirectory(Voxels)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${mathHeaders}
	${eventHeaders}
	${graphicsHeaders}
	${voxelHeaders}

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

	${currentDir}/Phoenix.hpp
	${currentDir}/SplashScreen.hpp
	${currentDir}/Game.hpp
	${currentDir}/DebugOverlay.hpp

	PARENT_SCOPE
)
