project(Engine)

add_subdirectory(ThirdParty)

add_subdirectory(Include/Quartz2)
add_subdirectory(Source)

add_library(${PROJECT_NAME} STATIC ${Headers} ${Sources})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2main Glad ImGui sol2 liblua c++experimental)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Include)
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/include
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/Glad/include
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/ImGui
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/ImGui/examples
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/lua
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/sol2/include
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/stb_image
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/stb_rectpack
)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE opengl32.lib)
endif()


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
