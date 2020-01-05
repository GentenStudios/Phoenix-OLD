cmake_minimum_required(VERSION 3.0)

project(Engine LANGUAGES CXX)

add_subdirectory(ThirdParty)

add_subdirectory(Include)
add_subdirectory(Source)

add_library(${PROJECT_NAME} STATIC ${Headers} ${Sources})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static Glad ImGui)

target_include_directories(${PROJECT_NAME} PRIVATE Include)
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/include
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/Glad/include
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/ImGui
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/stb_image
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/stb_rectpack
	${CMAKE_CURRENT_LIST_DIR}/ThirdParty/ImGui/examples
)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE opengl32.lib)
endif()

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)
	find_package(Threads REQUIRED)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES} GL)
endif()

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

