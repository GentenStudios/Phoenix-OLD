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

#include <Client/Client.hpp>
#include <Client/Crosshair.hpp>
#include <Client/Game.hpp>

#include <Common/Voxels/BlockRegistry.hpp>

#include <Common/Commander.hpp>
#include <Common/ContentLoader.hpp>

#include <Common/Position.hpp>
#include <Common/Actor.hpp>

using namespace phx::client;
using namespace phx;

static Commander kirk;

///@todo This needs refactored to play nicely
/**
* This exists so we can call the message function from the chat client,
* we definitely need to just clean that up so it all plays nicely. If
* a second instance of the game is created, this entire system will break
* (but so will a few others . . . )
*/
static Game* myGame = nullptr;

static void rawEcho(const std::string& input, std::ostringstream& cout)
{
	myGame->sendMessage(input, cout);
}

Game::Game(gfx::Window* window, entt::registry* registry)
: Layer("Game"), m_window(window), m_registry(registry)
{
	ContentManager::get()->lua["core"]["print"] =
		/**
		* @addtogroup luaapi
		*
		* @subsubsection coreprint core.print(text)
		* @brief Prints text to the players terminal
		*
		* @param text The text to be outputted to the terminal
		*
		*/
		[=](const std::string& text) { m_chat->cout << text << "\n"; };

	voxels::BlockRegistry::get()->initialise();

	myGame = this;
}

Game::~Game() { delete m_chat; }

void Game::onAttach()
{
	if (enet_initialize () != 0)
	{
		fprintf (stderr, "An error occurred while initializing ENet.\n");
		exit(0);
	}
	atexit (enet_deinitialize);

	m_client = enet_host_create (NULL /* the address to bind the server host to */,
							1      /* connect to a single server */,
							3      /* allow up to 3 channels to be used */,
							0      /* assume any amount of incoming bandwidth */,
							0      /* assume any amount of outgoing bandwidth */);
	if (m_client == NULL)
	{
		fprintf (stderr,
				"An error occurred while trying to create an ENet client host.\n");
		exit (EXIT_FAILURE);
	}

	enet_address_set_host(&m_address, "127.0.0.1");
	m_address.port = 7777;

	m_peer = enet_host_connect(m_client, &m_address, 3, 0);
	if (m_peer == NULL)
	{
		fprintf (stderr,
				"No available peers for initiating an ENet connection\n");
		exit (EXIT_FAILURE);
	}

	if(enet_host_service(m_client, &m_event, 5000) > 0 && m_event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to 127.0.0.1:7777 made");
	}
	else
	{
		enet_peer_reset(m_peer);
		puts("Connection to 127.0.0.1:7777 failed.");
	}

	m_chat = new ui::ChatWindow("Chat Window", 5,
								"Type /help for a command list and help.");

	/// @TODO replace with network callback
	m_chat->registerCallback(rawEcho);

	const std::string save = "save1";

    printf("%s", "Loading Modules\n");
	if (!ContentManager::get()->loadModules(save))
	{
		signalRemoval();
	}

    printf("%s", "Registering world\n");
	m_world  = new voxels::ChunkView(3, voxels::Map(save, "map1"));
	m_player = new Player(m_world, m_registry);
	m_camera = new gfx::FPSCamera(m_window, m_registry);
	m_camera->setActor(m_player->getEntity());

    m_registry->emplace<Hand>(m_player->getEntity(), voxels::BlockRegistry::get()->getFromRegistryID(0));

    printf("%s", "Prepare rendering\n");
	m_renderPipeline.prepare("Assets/SimpleWorld.vert",
							"Assets/SimpleWorld.frag",
							gfx::ChunkRenderer::getRequiredShaderLayout());

	m_renderPipeline.activate();

	const math::mat4 model;
	m_renderPipeline.setMatrix("u_model", model);

    printf("%s", "Register GUI\n");
	Client::get()->pushLayer(new Crosshair(m_window));
	m_escapeMenu = new EscapeMenu(m_window);

	if (Client::get()->isDebugLayerActive())
	{
		m_gameDebug = new GameTools(&m_followCam, &m_playerHand, m_player, m_registry);
		Client::get()->pushLayer(m_gameDebug);
	}
    printf("%s", "Game layer attached");
}

void Game::onDetach()
{
	enet_peer_disconnect(m_peer, 0);

	while(enet_host_service(m_client, &m_event, 3000) > 0)
	{
		switch(m_event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(m_event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			puts("Disconnection succeeded.");
			break;
		}
	}

	delete m_world;
	delete m_player;
	delete m_camera;
}

void Game::onEvent(events::Event& e)
{
	switch (e.type)
	{
	case events::EventType::KEY_PRESSED:
		switch (e.keyboard.key)
		{
		case events::Keys::KEY_ESCAPE:
			m_camera->enable(!m_camera->isEnabled());
			if (!m_camera->isEnabled())
			{
				Client::get()->pushLayer(m_escapeMenu);
			}
			else
			{
				Client::get()->popLayer(m_escapeMenu);
			}
			e.handled = true;
			break;
		case events::Keys::KEY_Q:
			m_window->close();
			e.handled = true;
			break;
		case events::Keys::KEY_E:
			m_playerHand++;
            m_registry->get<Hand>(m_player->getEntity()).hand =
			    voxels::BlockRegistry::get()->getFromRegistryID(m_playerHand);
			e.handled = true;
			break;
		case events::Keys::KEY_R:
			m_playerHand--;
            m_registry->get<Hand>(m_player->getEntity()).hand =
			    voxels::BlockRegistry::get()->getFromRegistryID(m_playerHand);
			e.handled = true;
			break;
		case events::Keys::KEY_P:
			if (Client::get()->isDebugLayerActive())
				if (m_gameDebug == nullptr)
				{
					m_gameDebug =
					    new GameTools(&m_followCam, &m_playerHand, m_player, m_registry);
					Client::get()->pushLayer(m_gameDebug);
				}
				else
				{
					Client::get()->pushLayer(m_gameDebug);
				}
			else
			{
				Client::get()->popLayer(m_gameDebug);
			}
			// don't set this to handled so we can propagate this down the
			// stack to enable debug overlays.
			// e.handled = true;
			break;
		default:
			break;
		}
		break;
	case events::EventType::MOUSE_BUTTON_PRESSED:
		if (!m_camera->isEnabled())
			break;
		switch (e.mouse.button)
		{
		case events::MouseButtons::LEFT:
			m_player->action1();
			e.handled = true;
			break;

		case events::MouseButtons::RIGHT:
			m_player->action2();
			e.handled = true;
			break;

		default:
			break;
		}
		break;
	case events::EventType::WINDOW_DEFOCUSED:
		m_camera->enable(false);
		break;
	case events::EventType::WINDOW_FOCUSED:
		m_camera->enable(true);
		break;
	default:
		break;
	}
}

void Game::tick(float dt)
{
	if(enet_host_service(m_client, &m_event, 0))
	{
		switch(m_event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			printf ("A packet of length %zu containing %s was received from %u on channel %u|%u.\n",
					m_event.packet -> dataLength,
					m_event.packet -> data,
					m_event.peer -> address.host,
					m_event.peer -> address.port,
			       m_event.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(m_event.packet);

			break;
		}
	}

	m_camera->tick(dt);

	char state[10];

	static std::size_t sequence;
	sequence++;
	state[0] = sequence;

	// WASD
	state[1] = 0;
	if (m_window->isKeyDown(events::Keys::KEY_W))
		state[1] |= 1 << 7;
	if (m_window->isKeyDown(events::Keys::KEY_S))
		state[1] |= 1 << 6;
	if (m_window->isKeyDown(events::Keys::KEY_A))
		state[1] |= 1 << 5;
	if (m_window->isKeyDown(events::Keys::KEY_D))
		state[1] |= 1 << 4;
	if (m_window->isKeyDown(events::Keys::KEY_SPACE))
		state[1] |= 1 << 3;
	if (m_window->isKeyDown(events::Keys::KEY_LEFT_SHIFT))
		state[1] |= 1 << 2;

	std::memcpy(state + 2,
	            &m_registry->get<Position>(m_player->getEntity()).rotation.x,
	            4);
	std::memcpy(state + 6,
	            &m_registry->get<Position>(m_player->getEntity()).rotation.y,
	            4);

	ENetPacket* packet;
	packet = enet_packet_create(&state, sizeof(state),
	                            ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);

	enet_peer_send(m_peer, 1, packet);
	enet_host_flush(m_client);

	if (m_followCam)
	{
		m_prevPos = m_registry->get<Position>(m_player->getEntity()).position;
	}

	m_world->tick(m_prevPos);

	m_chat->draw();

	m_renderPipeline.activate();
	m_renderPipeline.setMatrix("u_view", m_camera->calculateViewMatrix());
	m_renderPipeline.setMatrix("u_projection", m_camera->getProjection());

	m_world->render();
}

void Game::sendMessage(const std::string& input, std::ostringstream& cout)
{
	ENetPacket* packet;
	packet = enet_packet_create(input.c_str(), input.size(),
								ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_peer, 2, packet);
	enet_host_flush(m_client);
}
