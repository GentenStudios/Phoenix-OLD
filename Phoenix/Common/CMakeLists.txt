project(PhoenixCommon)

option(PHX_BUILD_TESTS OFF)

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

set_target_properties(${PROJECT_NAME} PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		)

if (PHX_BUILD_TESTS)
	add_subdirectory(Test)
	add_executable(${PROJECT_NAME}_test ${Headers} ${Sources} ${Tests})

	target_link_libraries(${PROJECT_NAME}_test
			PRIVATE
			${PHX_THIRD_PARTY_LIBRARIES}
			Catch2::Catch2
			$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
			)

	target_include_directories(${PROJECT_NAME}_test
			PRIVATE
			${PHX_THIRD_PARTY_INCLUDES}
			PUBLIC
			Include
			)

	set_target_properties(${PROJECT_NAME}_test PROPERTIES
			CXX_STANDARD 17
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			)
endif ()

#################################################
## ORGANISE FILES FOR IDEs (Xcode, VS, etc...) ##
#################################################

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include/Common" PREFIX "Header Files" FILES ${Headers})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source Files" FILES ${Sources})
