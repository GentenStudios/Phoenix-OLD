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

#include <Common/Commander.hpp>
#include <Common/ContentLoader.hpp>
#include <Common/Voxels/BlockRegistry.hpp>

#include <Common/Voxels/WorldGenerator.hpp>

using namespace phx::client;
using namespace phx;

static Commander kirk;

static void rawEcho(const std::string& input, std::ostringstream& cout)
{
	kirk.callback(input, cout);
}

Game::Game(gfx::Window* window) : Layer("Game"), m_window(window)
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

	ContentManager::get()->lua["world_params"] =
	    ContentManager::get()->lua.create_table();

	ContentManager::get()->lua["world_params"]["setWorldParams"] = [&] (float strength, float size, int octaves, float persistence, float height) {
		WorldGenerator::params = (WorldGenerator::Params{strength, size, octaves, persistence, height});
	};
	Voronoi::init();
}

Game::~Game() { delete m_chat; }

void Game::onAttach()
{
	m_chat = new ui::ChatWindow("Chat Window", 5,
	                            "Type /help for a command list and help.");

	m_chat->registerCallback(rawEcho);

	const std::string save = "save1";

	if (!ContentManager::get()->loadModules(save))
	{
		signalRemoval();
	}

	m_world  = new voxels::ChunkView(4, voxels::Map(save, "map1"));
	m_player = new Player(m_world);
	m_camera = new gfx::FPSCamera(m_window);
	m_camera->setActor(m_player);

	m_player->setHand(voxels::BlockRegistry::get()->getFromRegistryID(0));

	m_renderPipeline.prepare("Assets/SimpleWorld.vert",
	                         "Assets/SimpleWorld.frag",
	                         gfx::ChunkRenderer::getRequiredShaderLayout());

	m_renderPipeline.activate();

	const math::mat4 model;
	m_renderPipeline.setMatrix("u_model", model);

	Client::get()->pushLayer(new Crosshair(m_window));
	m_escapeMenu = new EscapeMenu(m_window);

	if (Client::get()->isDebugLayerActive())
	{
		m_gameDebug = new GameTools(&m_followCam, &m_playerHand, m_player);
		Client::get()->pushLayer(m_gameDebug);
	}
}

void Game::onDetach()
{
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
			m_player->setHand(
			    voxels::BlockRegistry::get()->getFromRegistryID(m_playerHand));
			e.handled = true;
			break;
		case events::Keys::KEY_R:
			m_playerHand--;
			m_player->setHand(
			    voxels::BlockRegistry::get()->getFromRegistryID(m_playerHand));
			e.handled = true;
			break;
		case events::Keys::KEY_P:
			if (Client::get()->isDebugLayerActive())
				if (m_gameDebug == nullptr)
				{
					m_gameDebug =
					    new GameTools(&m_followCam, &m_playerHand, m_player);
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
	m_camera->tick(dt);

	if (m_followCam)
	{
		m_prevPos = m_player->getPosition();
	}

	m_world->tick(m_prevPos);

	m_chat->draw();

	m_renderPipeline.activate();
	m_renderPipeline.setMatrix("u_view", m_camera->calculateViewMatrix());
	m_renderPipeline.setMatrix("u_projection", m_camera->getProjection());

	m_world->render();
}
