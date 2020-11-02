add_subdirectory(Graphics)
add_subdirectory(Audio)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
        ${graphicsSources}
        ${audioSources}

        ${currentDir}/InputQueue.cpp

        ${currentDir}/Client.cpp
        ${currentDir}/SplashScreen.cpp
        ${currentDir}/Crosshair.cpp
        ${currentDir}/Game.cpp
        ${currentDir}/EscapeMenu.cpp
        ${currentDir}/InputMap.cpp
        ${currentDir}/Network.cpp

        ${currentDir}/DebugOverlay.cpp
        ${currentDir}/GameTools.cpp
        ${currentDir}/InventoryUI.cpp

        ${currentDir}/Main.cpp

        PARENT_SCOPE
        )
