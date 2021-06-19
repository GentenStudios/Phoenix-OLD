project(NlohmannJson)

set(root ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE Headers ${root}/include/*.hpp)

add_library(${PROJECT_NAME} INTERFACE ${Headers})
target_include_directories(${PROJECT_NAME} INTERFACE ${root}/include)
