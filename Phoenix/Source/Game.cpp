// Copyright 2019 Genten Studios
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

#include <Phoenix/ContentLoader.hpp>
#include <Phoenix/Game.hpp>
#include <Phoenix/Voxels/BlockRegistry.hpp>

using namespace phx::client;
using namespace phx;

static Commander kirk;

static void rawEcho(const std::string& input, std::ostringstream& cout)
{
	kirk.callback(input, cout);
}

Game::Game(gfx::Window* window) : Layer("Game"), m_window(window)
{
	/// @todo fix chat window function pointer callback thingy and reimplement
	/// chat window into here.

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
}

Game::~Game() { delete m_chat; }

void Game::onAttach()
{
	m_chat = new ui::ChatWindow("Chat Window", 5,
	                            "Type /help for a command list and help.");

	m_chat->registerCallback(rawEcho);

	std::string save = "save1";

	if (!ContentManager::get()->loadModules(save))
	{
		signalRemoval();
	}

	m_world  = new voxels::ChunkManager(3, voxels::Map(save, "map1"));
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

		default:
			break;
		}
		break;

	case events::EventType::MOUSE_BUTTON_PRESSED:
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

	default:
		break;
	}
}

void Game::tick(float dt)
{
	m_camera->tick(dt);
	m_world->tick(m_camera->getPosition());

	m_chat->draw();

	m_renderPipeline.activate();
	m_renderPipeline.setMatrix("u_view", m_camera->calculateViewMatrix());
	m_renderPipeline.setMatrix("u_projection", m_camera->getProjection());

	m_world->render();
}
