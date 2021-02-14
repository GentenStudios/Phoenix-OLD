project(PhoenixServer)

add_subdirectory(Include/Server)
add_subdirectory(Source)

add_executable(${PROJECT_NAME} ${Headers} ${Sources})

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		PhoenixCommon
		PhoenixThirdParty
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		Include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

#################################################
## ORGANISE FILES FOR IDEs (Xcode, VS, etc...) ##
#################################################

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include/Server" PREFIX "Header Files" FILES ${Headers})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source Files" FILES ${Sources})

#################################################
## COPY SAVE, ASSETS, and MODULES TO BUILD DIR ##
#################################################

add_dependencies(${PROJECT_NAME} PhoenixModules-server)
