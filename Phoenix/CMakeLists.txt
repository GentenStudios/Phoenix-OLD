add_subdirectory(ThirdParty)

# propagate these variables down the line into the child CMakeLists.
set(PHX_THIRD_PARTY_LIBRARIES ${PHX_THIRD_PARTY_LIBRARIES})
set(PHX_THIRD_PARTY_INCLUDES ${PHX_THIRD_PARTY_INCLUDES})
set(PHX_COMMON_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/Common/Include)

add_subdirectory(Client)
add_subdirectory(Common)
add_subdirectory(Server)

add_subdirectory(Assets)
add_subdirectory(Modules)

# not doing PhoenixSaves-client because we need to remember to remove it once we can.
set_target_properties(PhoenixAssets-client PhoenixModules-client PhoenixModules-server PROPERTIES FOLDER Dependencies)
