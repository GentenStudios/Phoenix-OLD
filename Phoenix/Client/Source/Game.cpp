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
		LOG_FATAL("CMS") << "Error opening save file";
		exit(EXIT_FAILURE);
	}

	std::string input;
	while (std::getline(fileStream, input))
	{
		toLoad.push_back(input);
	}

	m_modManager = new cms::ModManager(toLoad, {"Modules"});

	voxels::BlockRegistry::get()->registerAPI(m_modManager);

	m_modManager->registerFunction(
	    "core.command.register",
	    [](std::string command, std::string help, sol::function f) {});

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
	m_chat = new ui::ChatWindow("Chat Window", 5,
	                            "Type /help for a command list and help.");

	/// @TODO replace with network callback
	m_chat->registerCallback(rawEcho);

	m_network = new client::Network(m_chat->cout);
	m_network->start();

	m_player = new Player(m_registry);
	m_player->registerAPI(m_modManager);

	float progress = 0.f;
	auto  result   = m_modManager->load(&progress);

	if (!result.ok)
	{
		LOG_FATAL("MODDING") << "An error has occured.";
		exit(EXIT_FAILURE);
	}

	LOG_INFO("MAIN") << "Registering world";
	const std::string save = "save1";
	m_world = new voxels::ChunkView(3, voxels::Map(save, "map1"));
	m_player->setWorld(m_world);
	m_camera = new gfx::FPSCamera(m_window, m_registry);
	m_camera->setActor(m_player->getEntity());

	m_registry->emplace<Hand>(
	    m_player->getEntity(),
	    voxels::BlockRegistry::get()->getFromRegistryID(0));

	LOG_INFO("MAIN") << "Prepare rendering";
	m_renderPipeline.prepare("Assets/SimpleWorld.vert",
	                         "Assets/SimpleWorld.frag",
	                         gfx::ChunkRenderer::getRequiredShaderLayout());

	m_renderPipeline.activate();

	const math::mat4 model;
	m_renderPipeline.setMatrix("u_model", model);

	LOG_INFO("MAIN") << "Register GUI";
	m_crosshair  = new Crosshair(m_window);
	m_escapeMenu = new EscapeMenu(m_window);
	Client::get()->pushLayer(m_crosshair);

	if (Client::get()->isDebugLayerActive())
	{
		m_gameDebug =
		    new GameTools(&m_followCam, &m_playerHand, m_player, m_registry);
		Client::get()->pushLayer(m_gameDebug);
	}
	LOG_INFO("MAIN") << "Game layer attached";
}

void Game::onDetach()
{
	m_network->stop();
	delete m_world;
	delete m_player;
	delete m_camera;
	delete m_network;
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
				Client::get()->popLayer(m_crosshair);
			}
			else
			{
				Client::get()->popLayer(m_escapeMenu);
				Client::get()->pushLayer(m_crosshair);
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
	// temp, will change in the future, based on game time
	static math::vec3 lightdir(0.f, -1.f, 0.f);
	static float      time = 0.f;

	time += dt;

	lightdir.y = std::sin(time);
	lightdir.x = std::cos(time);

	m_camera->tick(dt);

	static std::size_t sequence;
	sequence++;

	// Build input state
	// @todo move this to inputQueue.cpp
	{
		InputState inputState;
		inputState.sequence = sequence;
		inputState.forward  = m_window->isKeyDown(events::Keys::KEY_W);
		inputState.backward = m_window->isKeyDown(events::Keys::KEY_S);
		inputState.left     = m_window->isKeyDown(events::Keys::KEY_A);
		inputState.right    = m_window->isKeyDown(events::Keys::KEY_D);
		inputState.up       = m_window->isKeyDown(events::Keys::KEY_SPACE);
		inputState.down     = m_window->isKeyDown(events::Keys::KEY_LEFT_SHIFT);

		/// conversion from rad to 1/1000 of degrees
		inputState.rotation.x = static_cast<unsigned>(
		    m_registry->get<Position>(m_player->getEntity()).rotation.x *
		    360000.0);
		inputState.rotation.y = static_cast<unsigned>(
		    m_registry->get<Position>(m_player->getEntity()).rotation.y *
		    360000.0);

		m_network->sendState(inputState);
	}

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

void Game::sendMessage(std::string input, std::ostringstream& cout)
{
	m_network->sendMessage(input);
}
