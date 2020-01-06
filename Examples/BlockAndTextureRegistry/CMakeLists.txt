project(BlockAndTextureRegistry)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/Main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${EngineIncludeDirectory})
target_link_libraries(${PROJECT_NAME} Engine)
