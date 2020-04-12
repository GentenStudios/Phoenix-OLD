add_subdirectory(Graphics)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
        ${graphicsSources}

        ${currentDir}/Player.cpp
        ${currentDir}/InputQueue.cpp

        ${currentDir}/Client.cpp
        ${currentDir}/SplashScreen.cpp
        ${currentDir}/Crosshair.cpp
        ${currentDir}/Game.cpp
        ${currentDir}/EscapeMenu.cpp
        ${currentDir}/InputMap.cpp

        ${currentDir}/DebugOverlay.cpp
        ${currentDir}/GameTools.cpp

        ${currentDir}/Main.cpp

        PARENT_SCOPE
        )
