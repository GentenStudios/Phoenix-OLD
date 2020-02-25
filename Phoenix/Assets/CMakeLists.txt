project(PhoenixAssets)

set(assetsPath ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE assetFiles "*.*")

add_custom_target(${PROJECT_NAME}-client
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${assetsPath} ${CMAKE_BINARY_DIR}/Phoenix/Client/Assets
				   SOURCES ${assetFiles}
)
