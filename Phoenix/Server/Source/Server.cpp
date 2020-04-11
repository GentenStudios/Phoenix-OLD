// Copyright 2020 Genten Studios
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

#include <Common/Settings.hpp>
#include <Server/Server.hpp>

#include <Common/Math/Math.hpp>
#include <enet/enet.h>

#include <Common/Actor.hpp>
#include <Common/Movement.hpp>
#include <Common/Position.hpp>
#include <cstring>
#include <iostream>
#include <utility>

using namespace phx::server;
using namespace phx;

Server::Server(std::string save) : m_save(std::move(save))
{
	std::cout << "Hello, Server!" << std::endl;
	if (enet_initialize() != 0)
	{
		/// @TODO replace this with the logger
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		exit(0);
	}
	atexit(enet_deinitialize);

	m_address.host = ENET_HOST_ANY;
	m_address.port = 7777;

	m_server = enet_host_create(&m_address, maxUsers, 3, 0, 0);
	if (m_server == NULL)
	{
		fprintf(
		    stderr,
		    "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
}

void Server::run()
{
    Settings::get()->load("config.txt");
    m_running = true;
    while(m_running)
	{
		while (enet_host_service(m_server, &m_event, 1000) > 0)
		{
			switch (m_event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n",
				       m_event.peer->address.host, m_event.peer->address.port);
				{
					auto entity = m_registry.create();
					m_registry.emplace<User>(entity, "toby", m_event.peer);
					m_event.peer->data = static_cast<void*>(&entity);
					m_registry.emplace<Player>(
					    entity, ActorSystem::registerActor(&m_registry));
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
                    parseEvent(m_server, static_cast<entt::entity*>(m_event.peer->data), m_event.packet->data);
                    break;
                case 1:
                    parseState(m_server, static_cast<entt::entity*>(m_event.peer->data), m_event.packet->data);
                    break;
                case 2:
                    parseMessage(m_server, static_cast<entt::entity*>(m_event.peer->data), m_event.packet->data);
                    break;
                }

				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(m_event.packet);

				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", m_event.peer->data);
				break;
			case ENET_EVENT_TYPE_NONE:
				break;
			}
		}
	}

    Settings::get()->save("config.txt");
    enet_host_destroy(m_server);
}

Server::~Server()
{
    enet_host_destroy(m_server);
}

void Server::parseEvent(ENetHost* server, entt::entity* userRef,
                        enet_uint8* data)
{
	User user = m_registry.get<User>(*userRef);
	printf("Event received");
	printf("An Event packet containing %s was received from %s\n", data,
	       user.userName.c_str());
}

void Server::parseState(ENetHost* server, entt::entity* userRef,
                        enet_uint8* data)
{
	User user = m_registry.get<User>(*userRef);
	printf("A State packet containing %s was received from %s\n", data,
	       user.userName.c_str());
	auto  actorRef = m_registry.get<Player>(*userRef).actor;
	auto& pos      = m_registry.get<Position>(actorRef);

	std::memcpy(&pos.rotation.x, &data[1], 4);
	std::memcpy(&pos.rotation.y, &data[5], 4);

	const float moveSpeed =
	    static_cast<float>(m_registry.get<Movement>(actorRef).moveSpeed);
	const float      dt        = 1.f / 20.f;
	math::vec3       direction = pos.getDirection();
	const math::vec3 right     = {std::sin(direction.x - math::PIDIV2), 0.f,
                              std::cos(direction.x - math::PIDIV2)};
	const math::vec3 forward   = {std::sin(direction.x), 0.f,
                                std::cos(direction.x)};
	if (data[0] & static_cast<char>(1 << 7))
	{
		pos.position += forward * dt * moveSpeed;
	}
	else if (data[0] & static_cast<char>(1 << 6))
	{
		pos.position -= forward * dt * moveSpeed;
	}

	if (data[0] & static_cast<char>(1 << 5))
	{
		pos.position -= right * dt * moveSpeed;
	}
	else if (data[0] & static_cast<char>(1 << 4))
	{
		pos.position += right * dt * moveSpeed;
	}

	if (data[0] & static_cast<char>(1 << 3))
	{
		pos.position.y += dt * moveSpeed;
	}
	else if (data[0] & static_cast<char>(1 << 2))
	{
		pos.position.y -= dt * moveSpeed;
	}

	std::cout << pos.position << "\n";
}

void Server::parseMessage(ENetHost* server, entt::entity* userRef, enet_uint8 *data) {
    User user = m_registry.get<User>(*userRef);
    if (data[0] == '/'){
        printf("Received command %s from %s.", data, user.userName.c_str());
    } else {
        std::string message = user.userName + ": " + reinterpret_cast<char*>(data);

        printf("%s", message.c_str());
        ENetPacket * packet = enet_packet_create (message.c_str(), message.size(), ENET_PACKET_FLAG_RELIABLE);
        auto view = m_registry.view<User>();
        for(auto entity : view)
        {
            enet_peer_send (view.get<User>(entity).peer, 2, packet);
        }
        enet_host_flush(m_server);
    }
}
