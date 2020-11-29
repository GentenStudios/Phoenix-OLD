set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/Mod.hpp
	${currentDir}/ModAPI.hpp
	${currentDir}/ModManager.hpp
	${currentDir}/ModManager.inl

	PARENT_SCOPE
)
