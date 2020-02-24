project(PhoenixSaves)

set(savePath ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE saveFiles "*.*")

add_custom_target(${PROJECT_NAME}
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${savePath} ${CMAKE_CURRENT_BINARY_DIR}
				   SOURCES ${saveFiles}
)
