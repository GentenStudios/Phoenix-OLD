add_subdirectory(Graphics)
add_subdirectory(Game)
add_subdirectory(Renderer)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
        ${Sources}

        ${currentDir}/InputQueue.cpp

        ${currentDir}/Client.cpp
        ${currentDir}/SplashScreen.cpp
        ${currentDir}/Crosshair.cpp
        ${currentDir}/Game.cpp
        ${currentDir}/EscapeMenu.cpp
        ${currentDir}/HUD.cpp
        ${currentDir}/InputMap.cpp
        ${currentDir}/Network.cpp

        ${currentDir}/DebugOverlay.cpp
        ${currentDir}/GameTools.cpp

        ${currentDir}/Main.cpp

        PARENT_SCOPE
        )
