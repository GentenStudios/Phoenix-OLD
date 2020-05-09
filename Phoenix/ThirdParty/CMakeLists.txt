set(HAVE_LIBC TRUE)

# make SDL build ONLY the static lib, we don't use the shared lib.
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)

# stop SDL trying to control our main function.
add_definitions(-DSDL_MAIN_HANDLED)

# turn off the JSON tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

# turn off OpenAL-Soft tests/utilities
set(ALSOFT_UTILS OFF)
set(ALSOFT_NO_CONFIG_UTIL ON)
set(ALSOFT_EXAMPLES OFF)
set(ALSOFT_TESTS OFF)

add_subdirectory (openal-soft)
add_subdirectory (SDL2)
add_subdirectory (Glad)
add_subdirectory (ImGui)
add_subdirectory (sol2)
add_subdirectory (lua)
add_subdirectory (json)

# new line for each group of dependencies. Currently SDL, OpenGL, Lua and then OpenAL.
set_target_properties(SDL2main SDL2-static uninstall
		Glad ImGui
		lua liblua
		build_version native-tools
		OpenAL common ex-common
		PROPERTIES FOLDER Dependencies)

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
	${CMAKE_CURRENT_LIST_DIR}/json/include
	${CMAKE_CURRENT_LIST_DIR}/openal-soft/include
	${CMAKE_CURRENT_LIST_DIR}/minimp3

	PARENT_SCOPE
)

set(PHX_THIRD_PARTY_LIBRARIES
	SDL2-static 
	SDL2main
	Glad 
	ImGui
	sol2
	liblua
	nlohmann_json::nlohmann_json
	OpenAL
	$<$<PLATFORM_ID:Windows>:opengl32.lib> # link to opengl32.lib if windows.

	PARENT_SCOPE
)

target_compile_options(sol2 INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/wd4996>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-W>
)
