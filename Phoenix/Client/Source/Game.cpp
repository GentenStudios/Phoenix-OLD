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
#include <Common/Utility/Serializer.hpp>

#include <Common/Actor.hpp>
#include <Common/Commander.hpp>
#include <Common/Position.hpp>
#include <Common/Logger.hpp>
#include <Common/Movement.hpp>

#include <Common/PlayerView.hpp>
#include <cmath>
#include <tuple>

using namespace phx::client;
using namespace phx;

Game::Game(gfx::Window* window, entt::registry* registry, bool networked)
    : Layer("Game"), m_registry(registry), m_window(window)
{
	if (networked)
	{
		m_network = new client::Network(phx::net::Address("127.0.0.1", 7777));
		m_chat->setMessageCallback([this](const std::string& message)
		{
			m_network->sendMessage(message);
		});
	}
	// else TODO enable this else when we get mod list from network
	//{
	auto saveToUse = "save1";
	//}

	// use this as a placeholder until we have command line arguments.
	// even if the list is empty, it can create/load a save as required.
	// listing mods but loading an existing save will NOT load more mods, you
	// must manually edit the JSON to load an another mod after initialization.
	std::vector<std::string> commandLineModList = {"mod1", "mod2", "mod3"};
	m_save = new Save(saveToUse, commandLineModList);

	m_modManager = new cms::ModManager(m_save->getModList(), {"Modules"});

	m_blockRegistry.registerAPI(m_modManager);
	m_itemRegistry.registerAPI(m_modManager);

	m_modManager->registerFunction(
	    "core.command.register",
	    [](std::string command, std::string help, sol::function f) {});

	m_modManager->registerFunction("core.print", [=](const std::string& text) {
		m_network->sendMessage(text);
	});

	Settings::get()->registerAPI(m_modManager);
	InputMap::get()->registerAPI(m_modManager);
	CommandBook::get()->registerAPI(m_modManager);

	m_modManager->registerFunction("core.log_warning", [](std::string message) {
		LOG_WARNING("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_fatal", [](std::string message) {
		LOG_FATAL("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_info", [](std::string message) {
		LOG_INFO("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_debug", [](std::string message) {
		LOG_DEBUG("MODULE") << message;
	});
}

Game::~Game() { delete m_chat; }

void Game::onAttach()
{
	if (m_network != nullptr)
	{
		m_network->start();
	}

	ActorSystem::setBlockReferrer(&m_blockRegistry.referrer);
	m_player = ActorSystem::registerActor(m_registry);

	float progress = 0.f;
	auto  result   = m_modManager->load(&progress);

	if (!result.ok)
	{
		LOG_FATAL("MODDING") << "An error has occurred.";
		exit(EXIT_FAILURE);
	}

	LOG_INFO("MAIN") << "Registering world";
	if (m_network != nullptr)
	{
		m_map =
		    new voxels::Map(&m_network->chunkQueue, &m_blockRegistry.referrer);
	}
	else
	{
		m_map = new voxels::Map(m_save, "map1", &m_blockRegistry.referrer);
	}
	m_invManager =
	    new voxels::InventoryManager(m_save, &m_itemRegistry.referrer);
	m_playerInventory =
	    m_invManager->getInventory(m_invManager->createInventory(30));
	m_registry->emplace<Hand>(m_player, std::size_t(1), m_playerInventory);

	if (m_network)
	{
		m_chat = new gfx::ChatBox(m_window, &m_network->messageQueue);
	}
	else
	{
		m_chat = new gfx::ChatBox(m_window, nullptr);
	}

	m_registry->emplace<PlayerView>(m_player, m_map);
	m_camera = new gfx::FPSCamera(m_window, m_registry);
	m_camera->setActor(m_player);

	LOG_INFO("MAIN") << "Prepare rendering";

	m_mapRenderer =
	    new gfx::ChunkRenderer(m_map, &m_blockRegistry, m_registry, m_player);
	m_mapRenderer->attachCamera(m_camera);
	m_map->registerEventSubscriber(m_mapRenderer);
	m_mapRenderer->prep();

	m_renderPipeline.prepare("Assets/SimpleWorld.vert",
	                         "Assets/SimpleWorld.frag",
	                         gfx::ChunkRenderer::getRequiredShaderLayout());

	m_renderPipeline.activate();

	const math::mat4 model;
	m_renderPipeline.setMatrix("u_model", model);

	m_worldRenderer = new gfx::WorldRenderer();

	m_worldRenderer->setSkyboxTextures(
	    {"Assets/Skybox/north.png", "Assets/Skybox/west.png",
	     "Assets/Skybox/south.png", "Assets/Skybox/east.png",
	     "Assets/Skybox/zenith.png", "Assets/Skybox/nadir.png"});
	m_worldRenderer->attachCamera(m_camera);

	LOG_INFO("MAIN") << "Register GUI";
	m_escapeMenu = new EscapeMenu(m_window, m_camera);
	m_inventory  = new InventoryUI(m_window, m_camera, m_playerInventory,
                                  &m_itemRegistry.referrer);

	if (Client::get()->isDebugLayerActive())
	{
		m_gameDebug = new GameTools(&m_followCam, m_registry, m_player);
		Client::get()->pushLayer(m_gameDebug);
	}

	m_hud = new HUD(m_window, m_registry, m_player);
	Client::get()->pushLayer(m_hud);

	m_inputQueue = new InputQueue(m_registry, m_player, m_camera);
	if (m_network != nullptr)
	{
		m_inputQueue->start(std::chrono::milliseconds(50), m_network);
	}

	LOG_INFO("MAIN") << "Game layer attached";
}

void Game::onDetach()
{
	delete m_mapRenderer;
	delete m_inputQueue;
	delete m_network;
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
			Client::get()->pushLayer(m_escapeMenu);
			e.handled = true;
			break;

		case events::Keys::KEY_I:
			Client::get()->pushLayer(m_inventory);
			e.handled = true;
			break;

		case events::Keys::KEY_T:
			m_chat->setDrawBox(!m_chat->shouldDrawBox());
			e.handled = true;
			break;

		case events::Keys::KEY_Q:
			m_window->close();
			e.handled = true;
			break;
		case events::Keys::KEY_E:
		{
			auto& hand = m_registry->get<Hand>(m_player);
			hand.setHandSlot(hand.getHandSlot() - 1);
			e.handled = true;
			break;
		}
		case events::Keys::KEY_R:
		{
			auto& hand = m_registry->get<Hand>(m_player);
			hand.setHandSlot(hand.getHandSlot() + 1);
			e.handled = true;
			break;
		}
		case events::Keys::KEY_P:
			if (Client::get()->isDebugLayerActive())
				if (m_gameDebug == nullptr)
				{
					m_gameDebug =
					    new GameTools(&m_followCam, m_registry, m_player);
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
	case events::EventType::MOUSE_BUTTON_PRESSED:
		if (!m_camera->isEnabled())
			break;
		switch (e.mouse.button)
		{
		case events::MouseButtons::LEFT:
			ActorSystem::action1(m_registry, m_player);
			e.handled = true;
			break;

		case events::MouseButtons::RIGHT:
			ActorSystem::action2(m_registry, m_player);
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

	const Position& position = m_registry->get<Position>(m_player);

	m_camera->tick(dt);
	InputState state = m_inputQueue->getCurrentState();

	ActorSystem::tick(m_registry, m_player, dt, state);
	if (m_network != nullptr)
	{
		confirmState(position);
	}

	if (m_followCam)
	{
		m_prevPos = position.position;
	}

	m_renderPipeline.activate();
	m_renderPipeline.setInt("u_TexArray", 0);
	m_renderPipeline.setMatrix("u_view", m_camera->calculateViewMatrix());
	m_renderPipeline.setMatrix("u_projection", m_camera->getProjection());
	m_renderPipeline.setFloat("u_AmbientStrength", 0.7f);
	m_renderPipeline.setVector3("u_LightDir", lightdir);
	m_renderPipeline.setFloat("u_Brightness", 0.6f);

	m_mapRenderer->tick(dt);
	m_mapRenderer->renderSelectionBox();

	m_worldRenderer->tick(dt);

	m_chat->draw();
}

void Game::confirmState(const Position& position)
{
	// We can't iterate through a queue so we drain the queue to this list
	std::size_t i = m_inputQueue->m_queue.size();
	for (std::size_t j = 0; j <= i; j++)
	{
		m_states.push_back(m_inputQueue->m_queue.pop());
	}

	// If there are no confirmation states ready from the network, we are done
	if (m_network->stateQueue.empty())
	{
		return;
	}
	auto confirmation = m_network->stateQueue.pop();

	// Discard any inputStates older than the confirmationState
	while (!m_states.empty() && m_states.front().sequence < confirmation.second)
	{
		m_states.pop_front();
	}

	// Create a temporary position entity located at the confirmation position
	// and run all the states sent to the network since that sequence against
	// it.
	auto      entity = m_registry->create();
	Position& pos    = m_registry->emplace<Position>(
        entity, confirmation.first.rotation, confirmation.first.position);
	m_registry->emplace<Movement>(
	    entity, m_registry->get<Movement>(m_player).moveSpeed);
	for (const auto& inputState : m_states)
	{
		phx::ActorSystem::tick(m_registry, entity, 1.f / 20.f, inputState);
	}

	// Compare the temporary position object with the current player position.
	math::vec3 diff = position.position - pos.position;

	// If the temporary position object based on the confirmation is too far off
	// from our current position, update our current position.
	const float precision = .25f;
	if (diff.x > precision || diff.x < -precision || diff.y > precision ||
	    diff.y < -precision || diff.z > precision || diff.z < -precision)
	{
		m_registry->get<Position>(m_player).position = pos.position;
	}

	m_registry->destroy(entity);
}
