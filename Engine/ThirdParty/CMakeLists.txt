if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/SDL2/include)
	message("It seems like the Git Submodules have not been initialised and cloned, they will now be initialised and cloned as required.")
	execute_process(COMMAND git submodule update --init
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
endif()

add_subdirectory (SDL2)
add_subdirectory (Glad)
add_subdirectory (ImGui)
