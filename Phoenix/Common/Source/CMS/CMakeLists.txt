set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Sources
	${Sources}

	${currentDir}/Mod.cpp
	${currentDir}/ModAPI.cpp
	${currentDir}/ModManager.cpp

	PARENT_SCOPE
)
