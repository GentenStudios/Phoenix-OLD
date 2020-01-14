project(Phoenix)

add_subdirectory(ThirdParty)
add_subdirectory(Assets)

add_subdirectory(Include/Phoenix)
add_subdirectory(Source)

add_executable(${PROJECT_NAME} ${Headers} ${Sources})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PHX_THIRD_PARTY_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE Include ${PHX_THIRD_PARTY_INCLUDES})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CMAKE_CXX_STANDARD_REQUIRED ON
	CMAKE_CXX_EXTENSIONS OFF
)

add_dependencies(${PROJECT_NAME} PhoenixAssets)

#################################################
## ORGANISE FILES FOR IDEs (Xcode, VS, etc...) ##
#################################################

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Include/Phoenix" PREFIX "Header Files" FILES ${Headers})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source Files" FILES ${Sources})
