project(PhoenixModules)

set(modulesPath ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE moduleFiles "*.*")

add_custom_target(${PROJECT_NAME}-server
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${modulesPath} ${CMAKE_BINARY_DIR}/Phoenix/Server/Modules
				   SOURCES ${moduleFiles}
)

add_custom_target(${PROJECT_NAME}-client
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${modulesPath} ${CMAKE_BINARY_DIR}/Phoenix/Client/Modules
				   SOURCES ${moduleFiles}
)
