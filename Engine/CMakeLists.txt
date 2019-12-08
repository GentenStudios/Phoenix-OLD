project(Engine)

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

	${Source}/Matrix4x4.cpp
	${Source}/Ray.cpp
	${Source}/ChunkRenderer.cpp
	${Source}/ShaderCompiler.cpp
	${Source}/Game.cpp
	${Source}/OpenGL32.cpp
        ${Source}/Camera.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${Headers}/../)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/include
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/glad/include
)

target_link_libraries(${PROJECT_NAME} SDL2-static SDL2main opengl32.lib Glad)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
