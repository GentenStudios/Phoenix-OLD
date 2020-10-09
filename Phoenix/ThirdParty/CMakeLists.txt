set(HAVE_LIBC TRUE)

# make SDL build ONLY the static lib, we don't use the shared lib.
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)

# turn off the JSON tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

# turn off OpenAL-Soft tests/utilities
set(ALSOFT_UTILS OFF)
set(ALSOFT_NO_CONFIG_UTIL ON)
set(ALSOFT_EXAMPLES OFF)
set(ALSOFT_TESTS OFF)

# turn off the EnTT tests.
set(BUILD_TESTING OFF)

add_subdirectory (openal-soft)
add_subdirectory (SDL2)

add_subdirectory (Glad)
add_subdirectory (ImGui)
add_subdirectory (sol2)
add_subdirectory (lua)
add_subdirectory (enet)
add_subdirectory (entt)

# Stop SDL trying to control our main function.
target_compile_definitions(SDL2main INTERFACE SDL_MAIN_HANDLED)
# NOMINMAX setting required for std::min std::max to work properly on Windows.
target_compile_definitions(EnTT INTERFACE $<$<PLATFORM_ID:Windows>:NOMINMAX>)

add_subdirectory (json)

# new line for each group of dependencies. Currently SDL, OpenGL, Lua and then OpenAL.
set_target_properties(SDL2main SDL2-static uninstall
	Glad ImGui
	lua liblua
	enet aob build_version
	OpenAL common
	PROPERTIES FOLDER Dependencies)

# utility thing to make phoenix cmake tidier
set(PHX_THIRD_PARTY_INCLUDES
	${CMAKE_CURRENT_LIST_DIR}/SDL2/include
	${CMAKE_CURRENT_LIST_DIR}/Glad/include
	${CMAKE_CURRENT_LIST_DIR}/ImGui
	${CMAKE_CURRENT_LIST_DIR}/ImGui/examples
	${CMAKE_CURRENT_LIST_DIR}/lua
	${CMAKE_CURRENT_LIST_DIR}/enet/include
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
	enet
	sol2
	liblua
	nlohmann_json::nlohmann_json
	OpenAL
	$<$<PLATFORM_ID:Windows>:opengl32.lib> # link to opengl32.lib if windows.
	$<$<PLATFORM_ID:Windows>:ws2_32.lib> # link to ws2_32.lib if windows.
	$<$<PLATFORM_ID:Windows>:winmm.lib> # link to winmm.lib if windows.

	PARENT_SCOPE
)

target_compile_options(sol2 INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/wd4996>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-W>
)
