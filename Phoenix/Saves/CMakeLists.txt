project(PhoenixSaves)

set(savePath ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE saveFiles "*.*")

add_custom_target(${PROJECT_NAME}-server
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${savePath} ${CMAKE_BINARY_DIR}/Phoenix/Server/Saves
				   SOURCES ${saveFiles}
)

# temporary, remove once Client no longer needs Saves folder.
add_custom_target(${PROJECT_NAME}-client
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${savePath} ${CMAKE_BINARY_DIR}/Phoenix/Client/Saves
				   SOURCES ${saveFiles}
)
