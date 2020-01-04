project(Sandbox)

add_subdirectory(Assets)

set(Source ${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT_NAME}
	${Source}/Main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Engine/Include)
target_link_libraries(${PROJECT_NAME} Engine)

add_dependencies(${PROJECT_NAME} SandboxAssets)
