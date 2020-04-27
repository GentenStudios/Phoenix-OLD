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
#include <Common/Movement.hpp>
#include <Common/Position.hpp>
#include <Common/Serialization/Serializer.hpp>

#include <cstring> //For std::memcpy on non-mac machines

using namespace phx;
using namespace phx::server::networking;

/// @todo Replace this with the config system
static const std::size_t MAX_USERS = 32;

Iris::Iris(entt::registry* registry, bool* running)
    : m_registry(registry), m_running(running)
{
	if (enet_initialize() != 0)
	{
		/// @TODO replace this with the logger
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		exit(0);
	}
	atexit(enet_deinitialize);

	m_address.host = ENET_HOST_ANY;
	m_address.port = 7777;

	m_server = enet_host_create(&m_address, MAX_USERS, 3, 0, 0);
	if (m_server == NULL)
	{
		fprintf(
		    stderr,
		    "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
}

Iris::~Iris() { enet_host_destroy(m_server); }

void Iris::run()
{
	while (m_running)
	{
		while (enet_host_service(m_server, &m_event, 50) > 0 && m_running)
		{
			switch (m_event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n",
				       m_event.peer->address.host, m_event.peer->address.port);
				{
					auto entity = m_registry->create();
					m_registry->emplace<User>(entity, "toby", m_event.peer);
					m_event.peer->data = static_cast<void*>(&entity);
					m_registry->emplace<Player>(
					    entity, ActorSystem::registerActor(m_registry));
				}
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				//                printf ("A packet of length %u containing %s
				//                was received from %s on channel %u.\n",
				//                        m_event.packet -> dataLength,
				//                        m_event.packet -> data,
				//                        m_event.peer -> data -> userName,
				//                        m_event.channelID);

				switch (m_event.channelID)
				{
				case 0:
					parseEvent(static_cast<entt::entity*>(m_event.peer->data),
					           m_event.packet->data,
					           m_event.packet->dataLength);
					break;
				case 1:
					parseState(static_cast<entt::entity*>(m_event.peer->data),
					           m_event.packet->data,
					           m_event.packet->dataLength);
					break;
				case 2:
					parseMessage(static_cast<entt::entity*>(m_event.peer->data),
					             m_event.packet->data,
					             m_event.packet->dataLength);
					break;
				}

				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(m_event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n",
				       static_cast<const char*>(m_event.peer->data));
				break;

			case ENET_EVENT_TYPE_NONE:
				break;
			}
		}
	}
}

void Iris::auth() {}

void Iris::disconnect() {}
void Iris::parseEvent(entt::entity* userRef, enet_uint8* data, std::size_t dataLength)
{
	User user = m_registry->get<User>(*userRef);
	printf("Event received");
	printf("An Event packet containing %s was received from %s\n", data,
	       user.userName.c_str());
}
void Iris::parseState(entt::entity* userRef, enet_uint8* data, std::size_t dataLength)
{
	User user = m_registry->get<User>(*userRef);
	printf("A State packet containing %s was received from %s\n", data,
	       user.userName.c_str());

	const float dt = 1.f / 20.f;

	InputState input;

    phx::Serializer ser(Serializer::Mode::READ);
	ser.setBuffer((std::byte*)data, dataLength);
    ser & input;

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
	    stateQueue.end()->sequence - input.sequence < 10)
	{
		printf("discard %lu \n", input.sequence);
		return;
	}

	// Fill the stateBundles up to the current input sequence
	while ((input.sequence > stateQueue.end()->sequence &&
	        input.sequence - stateQueue.end()->sequence > 10) ||
	       stateQueue.end()->sequence == 255)
	{
		// Insert a new bundle if this is the first packet in this sequence
		StateBundle bundle;
		bundle.sequence = stateQueue.end()->sequence + 1;
		bundle.ready    = false;
		bundle.users    = 1; ///@todo We need to capture how many users we are
		/// expecting packets from
		stateQueue.push_back(bundle);
	}

	{
		printf("insert existing %lu \n", input.sequence);
		for (auto bundle : stateQueue)
		{
			if (bundle.sequence == input.sequence)
			{
				// Thread safety! If we said a bundle is ready, were too late
				if (!bundle.ready)
				{
					bundle.states[userRef] = input;
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
void Iris::parseMessage(entt::entity* userRef, enet_uint8* data, std::size_t dataLength)
{
	User user = m_registry->get<User>(*userRef);
	if (data[0] == '/')
	{
		printf("Received command %s from %s.", data, user.userName.c_str());
	}
	else
	{
		std::string message =
		    user.userName + ": " + reinterpret_cast<char*>(data);

		printf("%s", message.c_str());
		ENetPacket* packet = enet_packet_create(message.c_str(), message.size(),
		                                        ENET_PACKET_FLAG_RELIABLE);
		auto        view   = m_registry->view<User>();
		for (auto entity : view)
		{
			enet_peer_send(view.get<User>(entity).peer, 2, packet);
		}
		enet_host_flush(m_server);
	}
}
void Iris::sendEvent(entt::entity* userRef, enet_uint8* data) {}
void Iris::sendState(std::size_t sequence)
{
	auto view = m_registry->view<Position, Movement>();
	char state[13];
	for (auto entity : view)
	{
		std::memcpy(state, &sequence, 1);
		auto pos = view.get<Position>(entity);
		std::memcpy(state + 1, &pos.position.x, 4);
		std::memcpy(state + 5, &pos.position.y, 4);
		std::memcpy(state + 9, &pos.position.z, 4);
	}
	ENetPacket* packet = enet_packet_create(
	    state, sizeof(state), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);

	auto users = m_registry->view<User>();
	for (auto user : users)
	{
		enet_peer_send(users.get<User>(user).peer, 1, packet);
	}
	enet_host_flush(m_server);
}
void Iris::sendMessage(entt::entity* userRef, enet_uint8* data) {}
