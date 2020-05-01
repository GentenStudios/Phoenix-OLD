set(currentDir ${CMAKE_CURRENT_LIST_DIR})
set(networkSources
	${currentDir}/Address.cpp
	${currentDir}/Packet.cpp
	${currentDir}/Peer.cpp
	${currentDir}/Host.cpp

	PARENT_SCOPE
)
