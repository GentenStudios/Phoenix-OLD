project(Sandbox)

add_subdirectory(Assets)
add_subdirectory(Include/Sandbox)
add_subdirectory(Source)

add_executable(${PROJECT_NAME} ${Headers} ${Sources})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Engine/Include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Include)
target_link_libraries(${PROJECT_NAME} Engine)

add_dependencies(${PROJECT_NAME} SandboxAssets)

###########################################################
## THIS SORTS THE FILES FOR VISUAL STUDIO SOURCE GROUPS. ##
###########################################################

foreach(FILE ${Sources}) 
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    string(REPLACE "/" "\\" GROUP "${GROUP}")
	string(REPLACE "Source" "" GROUP "${GROUP}")

    set(GROUP "Source Files${GROUP}")
    source_group("${GROUP}" FILES "${FILE}")
endforeach()

foreach(FILE ${Headers})
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    string(REPLACE "/" "\\" GROUP "${GROUP}")
	string(REPLACE "Include" "" GROUP "${GROUP}")

    set(GROUP "Header Files${GROUP}")
    source_group("${GROUP}" FILES "${FILE}")
endforeach()
