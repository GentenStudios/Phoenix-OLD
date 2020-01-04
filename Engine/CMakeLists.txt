project(Engine)

# FIX: @ThirdParty @ImGUI
#   Properly specify which interface (ie. glad) to use with ImGui_ImplOpenGL3.h
#   otherwise it guesses which interface to use and may throw a linker error.
#
# NOTE:
#   Should replace -> https://cmake.org/cmake/help/latest/command/add_definitions.html
#   with this -> https://cmake.org/cmake/help/latest/command/add_compile_definitions.html#command:add_compile_definitions
#   if we migrate / update to the latest version of cmake.
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

add_subdirectory(ThirdParty)

set (Headers ${CMAKE_CURRENT_LIST_DIR}/Include/Quartz2)
set (Source ${CMAKE_CURRENT_LIST_DIR}/Source)

add_library(${PROJECT_NAME}
	${Headers}/Math.hpp
	${Headers}/MathUtils.hpp
	${Headers}/Matrix4x4.hpp
	${Headers}/Quartz.hpp
	${Headers}/Ray.hpp
	${Headers}/Rect.hpp
	${Headers}/Vec2.hpp
	${Headers}/Vec3.hpp
	${Headers}/ChunkRenderer.hpp
	${Headers}/ShaderCompiler.hpp
	${Headers}/Game.hpp
	${Headers}/OpenGL32.hpp
	${Headers}/Camera.hpp
	${Headers}/Chunk.hpp
	${Headers}/VoxelWorld.hpp
	${Headers}/Mesh.hpp
	${Headers}/Vertex.hpp
	${Headers}/BlocksTextureAtlas.hpp
	${Headers}/BlocksRegistry.hpp
	${Headers}/Singleton.hpp
	${Headers}/Settings.hpp

	${Source}/Matrix4x4.cpp
	${Source}/Ray.cpp
	${Source}/ChunkRenderer.cpp
	${Source}/ShaderCompiler.cpp
	${Source}/Game.cpp
	${Source}/OpenGL32.cpp
	${Source}/Camera.cpp
	${Source}/Chunk.cpp
	${Source}/VoxelWorld.cpp
	${Source}/Mesh.cpp
	${Source}/BlocksTextureAtlas.cpp
	${Source}/BlocksRegistry.cpp
	${Source}/Settings.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${Headers}/../)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/include
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/Glad/include
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/ImGui
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/stb_image
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/stb_rectpack
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/ImGui/examples
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main Glad ImGui)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE opengl32.lib)
endif()

