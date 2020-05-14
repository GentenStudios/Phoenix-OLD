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

#include <Server/Iris.hpp>
#include <Server/User.hpp>

#include <Common/Actor.hpp>
#include <Common/Logger.hpp>
#include <Common/Movement.hpp>
#include <Common/Position.hpp>
#include <Common/Serialization/Serializer.hpp>

#include <cstring> //For std::memcpy on non-mac machines

using namespace phx;
using namespace phx::net;
using namespace phx::server::net;

/// @todo Replace this with the config system
static const std::size_t MAX_USERS = 32;

Iris::Iris(entt::registry* registry) : m_registry(registry)
{
	m_server = new phx::net::Host(phx::net::Address(7777), MAX_USERS, 3);

	m_server->onConnect([this](Peer& peer, enet_uint32) {
		LOG_INFO("NETWORK")
		    << "Client connected from: " << peer.getAddress().getIP();
		{
			auto entity = m_registry->create();
			m_registry->emplace<Player>(entity,
			                            ActorSystem::registerActor(m_registry));
			m_users.emplace(peer.getID(), entity);
		}
	});

	m_server->onReceive(
	    [this](Peer& peer, Packet&& packet, enet_uint32 channelID) {
		    switch (channelID)
		    {
		    case 0:
			    parseEvent(peer.getID(), packet);
			    break;
		    case 1:
			    parseState(peer.getID(), packet);
			    break;
		    case 2:
			    parseMessage(peer.getID(), packet);
			    break;
		    }
	    });

	m_server->onDisconnect(
	    [this](std::size_t peerID, enet_uint32) { disconnect(peerID); });
}

Iris::~Iris() { delete m_server; }

void Iris::run()
{
	m_running = true;
	while (m_running)
	{
		m_server->poll();
	}
}

void Iris::auth() {}

void Iris::disconnect(std::size_t peerID)
{
	printf("%lu disconnected.\n", peerID);
}

void Iris::parseEvent(std::size_t userID, Packet& packet)
{
	std::string data;

	phx::Serializer ser(Serializer::Mode::READ);
	ser.setBuffer(packet.getData());
	ser& data;

	printf("Event received");
	printf("An Event packet containing %s was received from %lu\n",
	       data.c_str(), userID);
}

void Iris::parseState(std::size_t userID, phx::net::Packet& packet)
{
	const float dt = 1.f / 20.f;

	InputState input;

	auto data = packet.getData();

	phx::Serializer ser(Serializer::Mode::READ);
	ser.setBuffer(reinterpret_cast<std::byte*>(&data), packet.getSize());
	ser& input;

	// If the queue is empty we need to add a new bundle
	if (stateQueue.empty())
	{
		StateBundle bundle;
		bundle.sequence = input.sequence;
		bundle.ready    = false;
		bundle.users    = 1; ///@todo We need to capture how many users we are
		/// expecting packets from
		stateQueue.push_back(bundle);
	}

	// Discard state if its older that the oldest stateBundle
	if (input.sequence < stateQueue.front().sequence &&
	    stateQueue.back().sequence - input.sequence < 10)
	{
		printf("discard %lu \n", input.sequence);
		return;
	}

	// Fill the stateBundles up to the current input sequence
	while ((input.sequence > stateQueue.back().sequence &&
	        input.sequence - stateQueue.back().sequence > 10) ||
	       stateQueue.back().sequence == 255)
	{
		// Insert a new bundle if this is the first packet in this sequence
		StateBundle bundle;
		bundle.sequence = stateQueue.back().sequence + 1;
		bundle.ready    = false;
		bundle.users    = 1; ///@todo We need to capture how many users we are
		/// expecting packets from
		stateQueue.push_back(bundle);
	}

	{
		for (auto bundle : stateQueue)
		{
			if (bundle.sequence == input.sequence)
			{
				// Thread safety! If we said a bundle is ready, were too late
				if (!bundle.ready)
				{
					bundle.states[m_users[userID]] = input;
					// If we have all the states we need, then the bundle is
					// ready
					if (bundle.states.size() >= bundle.users)
					{
						bundle.ready = true;
					}
				}
				break;
			}
		}
	}

	// If we have more than 10 states enqueued, assume we lost a packet
	if (stateQueue.size() > 10)
	{
		stateQueue.front().ready = true;
	}
}

void Iris::parseMessage(std::size_t userID, phx::net::Packet& packet)
{
	std::string input;

	phx::Serializer ser(Serializer::Mode::READ);
	ser.setBuffer(packet.getData());
	ser& input;

	if (input[0] == '/')
	{
		MessageBundle message;
		message.message = input.substr(1);
		message.userID  = userID;
		messageQueue.push_back(message);
	}
	else
	{
		printf("%s", input.c_str());

		sendMessage(userID, input);
	}
}

void Iris::sendEvent(std::size_t userID, enet_uint8* data) {}

void Iris::sendState(entt::registry* registry, std::size_t sequence)
{
	auto       view = registry->view<Position, Movement>();
	Serializer ser(Serializer::Mode::WRITE);
	ser&       sequence;
	for (auto entity : view)
	{
		auto pos = view.get<Position>(entity);
		ser& pos.position.x& pos.position.y& pos.position.z;
	}
	Packet packet = Packet(ser.getBuffer(), PacketFlags::UNRELIABLE);
	m_server->broadcast(packet, 1);
}

void Iris::sendMessage(std::size_t userID, std::string message)
{
	Serializer ser(Serializer::Mode::WRITE);
	ser&       message;
	Packet     packet = Packet(ser.getBuffer(), PacketFlags::RELIABLE);
	Peer*      peer   = m_server->getPeer(userID);
	peer->send(packet, 3);
}
