set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
        ${Sources}

        ${currentDir}/Chunk.cpp
        ${currentDir}/Map.cpp
        ${currentDir}/Inventory.cpp
        ${currentDir}/InventoryManager.cpp

        PARENT_SCOPE
        )
