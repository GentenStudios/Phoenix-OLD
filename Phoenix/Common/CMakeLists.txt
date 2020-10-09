project(PhoenixCommon)

add_subdirectory(Include/Common)
add_subdirectory(Source)

add_library(${PROJECT_NAME} STATIC ${Headers} ${Sources})

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		${PHX_THIRD_PARTY_LIBRARIES}
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PHX_THIRD_PARTY_INCLUDES}
	PUBLIC
		Include

)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

#################################################
## ORGANISE FILES FOR IDEs (Xcode, VS, etc...) ##
#################################################

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include/Common" PREFIX "Header Files" FILES ${Headers})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source Files" FILES ${Sources})
