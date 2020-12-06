set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
		${Headers}

		${currentDir}/Block.hpp
		${currentDir}/BlockReferrer.hpp
		${currentDir}/Chunk.hpp
		${currentDir}/Inventory.hpp
		${currentDir}/Item.hpp
		${currentDir}/ItemReferrer.hpp
		${currentDir}/Map.hpp

	PARENT_SCOPE
)
