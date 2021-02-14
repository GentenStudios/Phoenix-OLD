# SDL2 setup
# Exposes targets SDL2main and SDL2-static
add_subdirectory(SDL2)

set(HAVE_LIBC TRUE)

# Make SDL build ONLY the static lib, we don't use the shared lib.
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)

# Stop SDL trying to control our main function.
target_compile_definitions(SDL2main INTERFACE SDL_MAIN_HANDLED)

# IDEFK what I am doing - this made SoLoud work. TODO: Fix this disaster
set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/SDL2/include)
set(SDL2_LIBRARY SDL2main)

# Glad setup
# Exposes target Glad
add_subdirectory(Glad)

# ImGui setup
# Exposes target ImGui
add_subdirectory(ImGui)

# sol2 setup
# Exposes target sol2::sol2
add_subdirectory(sol2)

target_compile_options(sol2 INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/wd4996>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-W>
)

# lua setup
# Exposes target liblua
add_subdirectory(lua)

# enet setup
# Exposes target enet
add_subdirectory(enet)

# EnTT setup
# Exposes target EnTT::EnTT
add_subdirectory(entt)

# NOMINMAX setting required for std::min std::max to work properly on Windows.
target_compile_definitions(EnTT INTERFACE $<$<PLATFORM_ID:Windows>:NOMINMAX>)

# Turn off the EnTT tests.
set(BUILD_TESTING OFF)

# Turn off the JSON tests.
set(JSON_BuildTests OFF CACHE INTERNAL "")

# JSON setup
# Exposes target nlohmann_json::nlohmann_json
add_subdirectory(json)

# Catch2 setup
# Exposes target Catch2::Catch2
add_subdirectory(Catch2)

# soloud setup
# Exposes target soloud
add_subdirectory(soloud/contrib)

# Soloud requires SSE3 instructions.
set_target_properties(soloud PROPERTIES
	COMPILE_OPTIONS "-msse3"
)

# Create interface target to represent all internal dependencies.
add_library(PhoenixThirdParty INTERFACE)
target_link_libraries(PhoenixThirdParty INTERFACE
	SDL2-static
	SDL2main
	Glad
	ImGui
	enet
	EnTT::EnTT
	sol2::sol2
	soloud
	liblua
	nlohmann_json::nlohmann_json

	$<$<PLATFORM_ID:Windows>:opengl32.lib> # link to opengl32.lib if windows.
	$<$<PLATFORM_ID:Windows>:ws2_32.lib> # link to ws2_32.lib if windows.
	$<$<PLATFORM_ID:Windows>:winmm.lib> # link to winmm.lib if windows.
)

target_include_directories(PhoenixThirdParty INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/Glad/include
	${CMAKE_CURRENT_LIST_DIR}/ImGui
	${CMAKE_CURRENT_LIST_DIR}/ImGui/examples
	${CMAKE_CURRENT_LIST_DIR}/enet/include
	${CMAKE_CURRENT_LIST_DIR}/soloud/include
	${CMAKE_CURRENT_LIST_DIR}/stb_image
	${CMAKE_CURRENT_LIST_DIR}/stb_rectpack
)

# new line for each group of dependencies. Currently SDL, OpenGL, Lua and then OpenAL.
set_target_properties(SDL2main SDL2-static uninstall
	Glad ImGui
	lua liblua
	enet aob
	PROPERTIES FOLDER Dependencies
)