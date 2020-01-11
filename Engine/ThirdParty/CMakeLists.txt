set(HAVE_LIBC TRUE)

# make SDL build ONLY the static lib, we don't use the shared lib.
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)

# stop SDL trying to control our main function.
add_definitions(-DSDL_MAIN_HANDLED)

add_subdirectory (SDL2)
add_subdirectory (Glad)
add_subdirectory (ImGui)
add_subdirectory (sol2)
add_subdirectory (lua)

set_target_properties(SDL2main SDL2-static uninstall Glad ImGui PROPERTIES FOLDER Dependencies)
