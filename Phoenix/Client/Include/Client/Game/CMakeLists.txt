set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
        ${Headers}

        ${currentDir}/Timestep.hpp
	${currentDir}/GameState.hpp
	${currentDir}/GameStateManager.hpp

        PARENT_SCOPE
        )
