project(Glad)

set(Root ${CMAKE_CURRENT_LIST_DIR})

add_library(${PROJECT_NAME}
	${Root}/include/glad/glad.h
	${Root}/include/KHR/khrplatform.h

	${Root}/src/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ${Root}/include)
