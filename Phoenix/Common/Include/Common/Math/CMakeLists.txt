set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/Math.hpp
	${currentDir}/MathUtils.hpp
	${currentDir}/Matrix4x4.hpp
	${currentDir}/Vector2.hpp
	${currentDir}/Vector3.hpp
	${currentDir}/Ray.hpp
	${currentDir}/Rect.hpp

	PARENT_SCOPE
)
