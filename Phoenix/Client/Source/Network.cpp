// Copyright 2019-20 Genten Studios
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <Client/Network.hpp>

#include <Common/Logger.hpp>
#include <Common/Voxels/Chunk.hpp>

using namespace phx::client;

Network::Network(std::ostringstream& chat, phx::net::Address address)
    : m_chat(chat)
{
	m_client = new phx::net::Host();

	m_client->onReceive([this](phx::net::Peer& peer, phx::net::Packet&& packet,
	                           enet_uint32 channelID) {
		switch (channelID)
		{
		case 0:
			parseEvent(packet);
			break;
		case 1:
			parseState(packet);
			break;
		case 2:
			parseMessage(packet);
			break;
		case 3:
			parseData(packet);
			break;
		default:
			LOG_WARNING("NETWORK")
			    << "Received Unexpected Packet on Channel " << channelID;
		}
	});

	m_client->onDisconnect([this](std::size_t peerID, enet_uint32) {
		std::cout << "Server disconnected";
	});

	m_client->connect(address, 4);
	m_client->poll(5000_ms);
}

Network::~Network()
{
	if (m_running)
		stop();
	delete m_client;
}

void Network::run()
{
	while (m_running)
	{
		m_client->poll(50_ms, 100);
	}
}

void Network::start()
{
	m_running = true;
    std::thread thread1 = std::thread(&Network::run, this);
    std::swap(m_thread, thread1);
}

void Network::stop()
{
	m_running = false;
	m_thread.join();
}

void Network::parseEvent(phx::net::Packet& packet)
{
	std::cout << "Event received";
}

void Network::parseState(phx::net::Packet& packet)
{
	auto data = packet.getData();

	phx::Serializer ser(Serializer::Mode::READ);
	ser.setBuffer(data.data(), packet.getSize());

	size_t sequence;
	ser&   sequence;
	if (sequence < m_currentSequence && sequence > 10)
	{
		return;
	}
	m_currentSequence = sequence;

	Position input;
	ser& input.position.x& input.position.y& input.position.z;

	stateQueue.push(std::pair(input, sequence));
}

void Network::parseMessage(phx::net::Packet& packet)
{
	std::string input;

	auto data = packet.getData();

	phx::Serializer ser(Serializer::Mode::READ);
	ser.setBuffer(reinterpret_cast<std::byte*>(data.data()), data.size());
	ser& input;

	LOG_INFO("Messenger") << input;
	m_chat << input;
	m_chat << "\n";
}

void Network::parseData(phx::net::Packet& packet)
{
	voxels::Chunk chunk(math::vec3 {0, 0, 0});

	auto data = packet.getData();

	phx::Serializer ser(Serializer::Mode::READ);
	ser.setBuffer(reinterpret_cast<std::byte*>(data.data()), data.size());
	ser& chunk;

	chunkQueue.push(chunk);
}

void Network::sendState(InputState inputState)
{
	Serializer ser(Serializer::Mode::WRITE);
	ser&       inputState;

	phx::net::Packet packet =
	    phx::net::Packet(ser.getBuffer(), phx::net::PacketFlags::UNRELIABLE);

	m_client->broadcast(packet, 1);
}

void Network::sendMessage(std::string message)
{
	Serializer       ser(Serializer::Mode::WRITE);
	ser&             message;
	phx::net::Packet packet =
	    phx::net::Packet(ser.getBuffer(), phx::net::PacketFlags::RELIABLE);
	m_client->broadcast(packet, 2);
}