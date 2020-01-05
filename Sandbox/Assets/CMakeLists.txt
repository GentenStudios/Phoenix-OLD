project(SandboxAssets)

set(assetsPath ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE assetFiles "*.*")

add_custom_target(${PROJECT_NAME}
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${assetsPath} ${CMAKE_CURRENT_BINARY_DIR}
				   SOURCES ${assetFiles}
)