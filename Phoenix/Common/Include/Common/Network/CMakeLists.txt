set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(Headers
	${Headers}

	${currentDir}/Types.hpp
	${currentDir}/Address.hpp
	${currentDir}/Peer.hpp
	${currentDir}/Packet.hpp
	${currentDir}/Host.hpp

	PARENT_SCOPE
)
