set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(serializationHeaders
        ${currentDir}/Endian.hpp
        ${currentDir}/Serializer.hpp
        ${currentDir}/Serializer.inl

        PARENT_SCOPE
        )
