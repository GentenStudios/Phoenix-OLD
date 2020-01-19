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
<<<<<<< HEAD
	${currentDir}/Player.hpp
	${currentDir}/Actor.hpp
=======
	${currentDir}/Commander.hpp
>>>>>>> b56ecfc282529ad650aa88ea04eb0c9e800eb6b0

	PARENT_SCOPE
)
