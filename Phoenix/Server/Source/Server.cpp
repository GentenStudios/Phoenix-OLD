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

#include <Server/Server.hpp>

#include <enet/enet.h>

#include <iostream>
#include <utility>

using namespace phx::server;
using namespace phx;

Server::Server(std::string save) : m_save(std::move(save))
{
    std::cout << "Hello, Server!" << std::endl;
    if (enet_initialize () != 0)
    {
        /// @TODO replace this with the logger
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        exit(0);
    }
    atexit (enet_deinitialize);

    m_address.host = ENET_HOST_ANY;
    m_address.port = 7777;

    /// @TODO when server config is set up, replace 32 with a max # of clients
    m_server = enet_host_create (&m_address /* the address to bind the server host to */,
                                 32      /* allow up to 32 clients and/or outgoing connections */,
                                 2      /* allow up to 2 channels to be used, 0 and 1 */,
                                 0      /* assume any amount of incoming bandwidth */,
                                 0      /* assume any amount of outgoing bandwidth */);
    if (m_server == NULL)
    {
        fprintf (stderr,
                 "An error occurred while trying to create an ENet server host.\n");
        exit (EXIT_FAILURE);
    }
}

void Server::run()
{
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
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				printf("A packet of length %zu containing %s was received from %s on channel %u.\n",
				       m_event.packet->dataLength, m_event.packet->data,
				       m_event.peer->data, m_event.channelID);
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(m_event.packet);

				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", m_event.peer->data);
			}
		}
	}

    enet_host_destroy(m_server);
}
Server::~Server()
{
    enet_host_destroy(m_server);
}
