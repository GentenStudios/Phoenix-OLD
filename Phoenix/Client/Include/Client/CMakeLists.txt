add_subdirectory(Events)
add_subdirectory(Graphics)
add_subdirectory(Audio)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
        ${eventHeaders}
        ${graphicsHeaders}
        ${audioHeaders}

        ${currentDir}/Player.hpp
        ${currentDir}/InputQueue.hpp

        ${currentDir}/Client.hpp
        ${currentDir}/SplashScreen.hpp
        ${currentDir}/Crosshair.hpp
        ${currentDir}/Game.hpp
        ${currentDir}/EscapeMenu.hpp
        ${currentDir}/InputMap.hpp
        ${currentDir}/Network.hpp

        ${currentDir}/DebugOverlay.hpp
        ${currentDir}/GameTools.hpp

        PARENT_SCOPE
        )
