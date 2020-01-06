project(Examples)

set(EngineIncludeDirectory ${CMAKE_CURRENT_LIST_DIR}/../Engine/Include)

add_subdirectory(BlockAndTextureRegistry)
add_subdirectory(WindowSimple)

set_target_properties(BlockAndTextureRegistry WindowSimple PROPERTIES FOLDER Examples)
