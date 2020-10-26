add_subdirectory(Internal)

set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(utilityHeaders
	${currentDir}/BlockingQueue.hpp

		${currentDir}/Metadata.hpp
		${currentDir}/Serializer.hpp
        ${currentDir}/Serializer.inl

        PARENT_SCOPE
)
