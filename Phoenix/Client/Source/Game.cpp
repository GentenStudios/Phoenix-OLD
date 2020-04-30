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

#include <Common/CMS/ModManager.hpp>
#include <Common/Serialization/Serializer.hpp>
#include <Common/Voxels/BlockRegistry.hpp>

#include <Common/Actor.hpp>
#include <Common/Commander.hpp>
#include <Common/Position.hpp>
#include <Common/Logger.hpp>

#include <cmath>

using namespace phx::client;
using namespace phx;

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
	const std::string save = "save1";

	std::fstream             fileStream;
	std::vector<std::string> toLoad;

	fileStream.open("Saves/" + save + "/Mods.txt");
	if (!fileStream.is_open())
	{
		std::cout << "Error opening save file";
		exit(EXIT_FAILURE);
	}

	std::string input;
	while (std::getline(fileStream, input))
	{
		toLoad.push_back(input);
	}

	m_modManager = new cms::ModManager(toLoad, {"Modules"});

	voxels::BlockRegistry::get()->registerAPI(m_modManager);

	m_modManager->registerFunction("core.print", [=](const std::string& text) {
		m_chat->cout << text << "\n";
	});

	Settings::get()->registerAPI(m_modManager);
	InputMap::get()->registerAPI(m_modManager);
	CommandBook::get()->registerAPI(m_modManager);

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

	m_player = new Player(m_registry);
	m_player->registerAPI(m_modManager);

	float progress = 0.f;
	auto  result   = m_modManager->load(&progress);

	if (!result.ok)
	{
		LOG_FATAL("MODDING") << "An error has occured.";
		exit(EXIT_FAILURE);
	}

	printf("%s", "Registering world\n");
	const std::string save = "save1";
	m_world  = new voxels::ChunkView(3, voxels::Map(save, "map1"));
	m_player->setWorld(m_world);
	m_camera = new gfx::FPSCamera(m_window, m_registry);
	m_camera->setActor(m_player->getEntity());

	m_registry->emplace<Hand>(
	    m_player->getEntity(),
	    voxels::BlockRegistry::get()->getFromRegistryID(0));

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
		m_gameDebug =
		    new GameTools(&m_followCam, &m_playerHand, m_player, m_registry);
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
					m_gameDebug = new GameTools(&m_followCam, &m_playerHand,
					                            m_player, m_registry);
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
	//	std::cout << "tick";
	//  LOG_WARNING("MAINLOOP") << "WTF";
	if (enet_host_service(m_client, &m_event, 0))
	{
		switch (m_event.type)
		{
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
				parseEvent(m_event.packet->data, m_event.packet->dataLength);
				break;
			case 1:
				parseState(m_event.packet->data, m_event.packet->dataLength);
				break;
			case 2:
				parseMessage(m_event.packet->data, m_event.packet->dataLength);
				break;
			}

			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(m_event.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconnected.\n",
			       static_cast<const char*>(m_event.peer->data));
			/// @todo we should probably kill the game at this point
			break;

		case ENET_EVENT_TYPE_NONE:
			break;
		}
	}

	// temp, will change in the future, based on game time
	static math::vec3 lightdir(0.f, -1.f, 0.f);
	static float time = 0.f;

	time += dt;

	lightdir.y = std::sin(time);
	lightdir.x = std::cos(time);

	m_camera->tick(dt);

	static std::size_t sequence;
	sequence++;

	// Build input state
	// @todo move this to inputQueue.cpp @beeper

	InputState inputState;
	inputState.sequence = sequence;
	inputState.forward  = m_window->isKeyDown(events::Keys::KEY_W);
	inputState.backward = m_window->isKeyDown(events::Keys::KEY_S);
	inputState.left     = m_window->isKeyDown(events::Keys::KEY_A);
	inputState.right    = m_window->isKeyDown(events::Keys::KEY_D);
	inputState.up       = m_window->isKeyDown(events::Keys::KEY_SPACE);
	inputState.down     = m_window->isKeyDown(events::Keys::KEY_LEFT_SHIFT);

	/// conversion from rad to 1/1000 of degres
	inputState.rotation.x =
	    static_cast<unsigned>(m_registry->get<Position>(m_player->getEntity()).rotation.x * 360000.0);
	inputState.rotation.y =
        static_cast<unsigned>(m_registry->get<Position>(m_player->getEntity()).rotation.y * 360000.0);

	phx::Serializer ser(Serializer::Mode::WRITE);
	auto state = ser & inputState & Serializer::endp;

	ENetPacket* packet;
	packet = enet_packet_create(state.data(), state.size(),
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
	m_renderPipeline.setFloat("u_AmbientStrength", 0.7f);
	m_renderPipeline.setVector3("u_LightDir", lightdir);
	m_renderPipeline.setFloat("u_Brightness", 0.6f);

	m_world->render();
	m_player->renderSelectionBox(m_camera->calculateViewMatrix(), m_camera->getProjection());
}

void Game::sendMessage(const std::string& input, std::ostringstream& cout)
{
	ENetPacket* packet;
	packet = enet_packet_create(input.c_str(), input.size(),
	                            ENET_PACKET_FLAG_RELIABLE);
	LOG_INFO("TEST") << "Send Message: " << input;
	enet_peer_send(m_peer, 2, packet);
	LOG_INFO("TEST") << "Sending Message";
	enet_host_flush(m_client);
	LOG_INFO("TEST") << "Sent Message";
}
void Game::parseEvent(enet_uint8* data, std::size_t dataLength)
{
	printf("Event received");
}
void Game::parseState(enet_uint8* data, std::size_t dataLength)
{
	printf("State received");
}
void Game::parseMessage(enet_uint8* data, std::size_t dataLength)
{
	LOG_INFO("Messenger") << data;
	m_chat->cout << data;
}
