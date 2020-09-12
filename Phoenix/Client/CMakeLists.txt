project(PhoenixClient)

add_subdirectory(Include/Client)
add_subdirectory(Source)

add_executable(${PROJECT_NAME} ${Headers} ${Sources})
target_link_libraries(${PROJECT_NAME}
                      PRIVATE ${PHX_THIRD_PARTY_LIBRARIES} PhoenixCommon
                      # Link <filesystem> for versions of GCC < 9.0
                      PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)

target_include_directories(${PROJECT_NAME}
                           PRIVATE Include
                           PRIVATE ${PHX_COMMON_INCLUDES} ${PHX_THIRD_PARTY_INCLUDES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CMAKE_CXX_STANDARD_REQUIRED ON
	CMAKE_CXX_EXTENSIONS OFF
)

#################################################
## ORGANISE FILES FOR IDEs (Xcode, VS, etc...) ##
#################################################

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include/Client" PREFIX "Header Files" FILES ${Headers})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source Files" FILES ${Sources})

#################################################
## COPY SAVE, ASSETS, and MODULES TO BUILD DIR ##
#################################################

add_dependencies(${PROJECT_NAME} PhoenixAssets-client)
add_dependencies(${PROJECT_NAME} PhoenixModules-client)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:OpenAL>
		$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_NAME:OpenAL>
)

