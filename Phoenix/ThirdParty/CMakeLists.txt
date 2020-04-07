set(HAVE_LIBC TRUE)

# FIX: @ThirdParty @ImGUI
#   Properly specify which interface (ie. glad) to use with ImGui_ImplOpenGL3.h
#   otherwise it guesses which interface to use and may throw a linker error.
#
# NOTE:
#   Should replace -> https://cmake.org/cmake/help/latest/command/add_definitions.html
#   with this -> https://cmake.org/cmake/help/latest/command/add_compile_definitions.html#command:add_compile_definitions
#   if we migrate / update to the latest version of cmake.
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

# make SDL build ONLY the static lib, we don't use the shared lib.
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)

# stop SDL trying to control our main function.
add_definitions(-DSDL_MAIN_HANDLED)

add_subdirectory (SDL2)
add_subdirectory (Glad)
add_subdirectory (ImGui)
add_subdirectory (sol2)
add_subdirectory (lua)

set_target_properties(SDL2main SDL2-static uninstall Glad ImGui lua liblua PROPERTIES FOLDER Dependencies)

# utility thing to make phoenix cmake tidier
set(PHX_THIRD_PARTY_INCLUDES
	${CMAKE_CURRENT_LIST_DIR}/SDL2/include
	${CMAKE_CURRENT_LIST_DIR}/Glad/include
	${CMAKE_CURRENT_LIST_DIR}/ImGui
	${CMAKE_CURRENT_LIST_DIR}/ImGui/examples
	${CMAKE_CURRENT_LIST_DIR}/lua
	${CMAKE_CURRENT_LIST_DIR}/sol2/include
	${CMAKE_CURRENT_LIST_DIR}/stb_image
	${CMAKE_CURRENT_LIST_DIR}/stb_rectpack
	${CMAKE_CURRENT_LIST_DIR}/entt/src

	PARENT_SCOPE
)

set(PHX_THIRD_PARTY_LIBRARIES
	SDL2-static 
	SDL2main
	Glad 
	ImGui
	sol2
	liblua
	$<$<PLATFORM_ID:Windows>:opengl32.lib> # link to opengl32.lib if windows.

	PARENT_SCOPE
)