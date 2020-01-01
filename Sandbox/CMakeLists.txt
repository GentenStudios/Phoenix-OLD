project(Sandbox)

set(Source ${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT_NAME}
	${Source}/UI.cpp
	${Source}/UI.hpp

	${Source}/Main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Engine/Include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} Engine)

execute_process (COMMAND
	${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_LIST_DIR}/Assets ${PROJECT_BINARY_DIR}/Assets)
