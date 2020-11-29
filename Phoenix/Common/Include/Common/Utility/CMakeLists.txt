add_subdirectory(Internal)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/BlockingQueue.hpp

        ${currentDir}/Serializer.hpp
        ${currentDir}/Serializer.inl

        PARENT_SCOPE
)
