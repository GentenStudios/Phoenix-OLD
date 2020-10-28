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

	m_modManager->registerFunction(
	    "core.command.register",
	    [](std::string command, std::string help, sol::function f) {});

	m_modManager->registerFunction("core.print", [=](const std::string& text) {
		m_network->sendMessage(text);
	});

	m_audio    = Client::get()->getAudioHandler();
	m_listener = m_audio->getListener();

	// play background music
	auto handle = m_audio->loadMP3("core:background_music1",
	                               "Assets/Audio/background_music.mp3");

	audio::Source backMusic((*m_audio)[handle]);
	backMusic.enableLoop(true);

	// background music shouldn't be spatial.
	backMusic.enableSpatial(false);

	backMusic.setGain(0.1f);

	Client::get()->getAudioPool()->queue(backMusic);

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

	m_modManager->registerFunction(
	    "audio.loadMP3",
	    [=](const std::string& uniqueName, const std::string& filePath) {
		    return Client::get()->getAudioHandler()->loadMP3(uniqueName,
		                                                     filePath);
	    });

	m_modManager->registerFunction("audio.play", [=](sol::table source) {
		audio::Source audioSource;

		// set which piece of audio is being played.
		sol::optional<std::string> isString = source["id"];
		if (isString)
		{
			audioSource.setAudioData(
			    (*Client::get()->getAudioHandler())[*isString]);
		}
		else
		{
			sol::optional<unsigned int> isInt = source["id"];
			if (isInt)
			{
				audioSource.setAudioData(
				    (*Client::get()->getAudioHandler())[*isInt]);
			}
		}

		// set if the audio is spatial (no idea what the default is, we should
		// find out).
		sol::optional<bool> isBool = source["spatial"];
		if (isBool)
		{
			audioSource.enableSpatial(*isBool);
		}
		else
		{
			audioSource.enableSpatial(true);
		}

		// set position.
		if (source["position"])
		{
			float x = source["position"]["x"];
			float y = source["position"]["y"];
			float z = source["position"]["z"];

			audioSource.setPos({x, y, z});
		}

		// set direction, if nothing, don't do anything since openal is
		// omnidirectional by default.
		if (source["direction"])
		{
			sol::optional<float> doesXExist = source["direction"]["x"];
			if (doesXExist)
			{
				float x = source.get_or(std::tie("direction", "x"), 0.f);
				float y = source.get_or(std::tie("direction", "y"), 0.f);
				float z = source.get_or(std::tie("direction", "z"), 0.f);
				audioSource.setDirection({x, y, z});
			}
			else
			{
				sol::optional<std::string> isWorldDirection =
				    source["direction"];

				if (isWorldDirection)
				{
					if (*isWorldDirection == "north")
					{
						audioSource.setDirection({0.f, 0.f, -1.f});
					}
					if (*isWorldDirection == "south")
					{
						audioSource.setDirection({0.f, 0.f, 1.f});
					}
					if (*isWorldDirection == "east")
					{
						audioSource.setDirection({1.f, 0.f, 0.f});
					}
					if (*isWorldDirection == "west")
					{
						audioSource.setDirection({-1.f, 0.f, 0.f});
					}
					if (*isWorldDirection == "up")
					{
						audioSource.setDirection({0.f, 1.f, 0.f});
					}
					if (*isWorldDirection == "down")
					{
						audioSource.setDirection({0.f, -1.f, 0.f});
					}
				}
			}
		}

		// set the gain, 1.0 by default.
		sol::optional<float> isFloat = source["gain"];
		if (isBool)
		{
			audioSource.setGain(*isFloat);
		}

		// set the pitch, 1.0 by default.
		sol::optional<float> isFloatPitch = source["pitch"];
		if (isFloatPitch)
		{
			audioSource.setPitch(*isFloatPitch);
		}

		// set looping status, disabled by default.
		sol::optional<bool> isLooped = source["loop"];
		if (isLooped)
		{
			audioSource.enableLoop(*isLooped);
		}

		Client::get()->getAudioPool()->queue(audioSource);
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
		m_map = new voxels::Map(&m_network->chunkQueue, &m_blockRegistry.referrer);
	}
	else
	{
		m_map = new voxels::Map(m_save, "map1", &m_blockRegistry.referrer);
	}

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

	m_registry->emplace<Hand>(
	    m_player, m_blockRegistry.referrer.blocks.get(0));

	LOG_INFO("MAIN") << "Prepare rendering";

	m_worldRenderer =
	    new gfx::ChunkRenderer(m_map, &m_blockRegistry, m_registry, m_player);
	m_worldRenderer->attachCamera(m_camera);
	m_map->registerEventSubscriber(m_worldRenderer);
	m_worldRenderer->prep();

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
		m_gameDebug = new GameTools(&m_followCam, m_registry, m_player);
		Client::get()->pushLayer(m_gameDebug);
	}

	m_inputQueue = new InputQueue(m_registry, m_player, m_camera);
	if (m_network != nullptr)
	{
		m_inputQueue->start(std::chrono::milliseconds(50), m_network);
	}

	LOG_INFO("MAIN") << "Game layer attached";
}

void Game::onDetach()
{
	delete m_worldRenderer;
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

		case events::Keys::KEY_T:
			m_chat->setDrawBox(!m_chat->shouldDrawBox());
			e.handled = true;
			break;

		case events::Keys::KEY_Q:
			m_window->close();
			e.handled = true;
			break;
		case events::Keys::KEY_E:
			m_playerHand++;
			m_registry->get<Hand>(m_player).hand =
			    m_blockRegistry.referrer.blocks.get(m_playerHand);
			e.handled = true;
			break;
		case events::Keys::KEY_R:
			m_playerHand--;
			m_registry->get<Hand>(m_player).hand =
			    m_blockRegistry.referrer.blocks.get(m_playerHand);
			e.handled = true;
			break;
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
		break;
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

	m_listener->setPosition(position.position);
	m_listener->setVelocity({0, 0, 0});

	m_renderPipeline.activate();
	m_renderPipeline.setInt("u_TexArray", 0);
	m_renderPipeline.setMatrix("u_view", m_camera->calculateViewMatrix());
	m_renderPipeline.setMatrix("u_projection", m_camera->getProjection());
	m_renderPipeline.setFloat("u_AmbientStrength", 0.7f);
	m_renderPipeline.setVector3("u_LightDir", lightdir);
	m_renderPipeline.setFloat("u_Brightness", 0.6f);

	m_worldRenderer->tick(dt);
	m_worldRenderer->renderSelectionBox();

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
