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

#include <Phoenix/Commander.hpp>
#include <Phoenix/ContentLoader.hpp>
#include <Phoenix/Graphics/Camera.hpp>
#include <Phoenix/Graphics/ChunkMesher.hpp>
#include <Phoenix/Graphics/ChunkRenderer.hpp>
#include <Phoenix/Graphics/Window.hpp>
#include <Phoenix/ImGuiHelpers.hpp>
#include <Phoenix/Player.hpp>
#include <Phoenix/Settings.hpp>
#include <Phoenix/Voxels/BlockRegistry.hpp>
#include <Phoenix/Voxels/ChunkManager.hpp>

#include <Phoenix/UI.hpp>

#include <imgui.h>

#include <sstream>
#include <string>

using namespace phx;

Commander kirk = Commander();

static void rawEcho(const std::string& input, std::ostringstream& cout)
{
	kirk.callback(input, cout);
}

static ui::ChatWindow chat("Chat Window", 5,
                           "Type something and hit enter to run a command!\n");

class Phoenix : public events::IEventListener
{
public:
	Phoenix()
	{
		m_window = new gfx::Window("Phoenix Game!", 1280, 720);
		m_window->registerEventListener(this);

		chat.registerCallback(&rawEcho);
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
	    [](std::string text) { chat.cout << text << "\n"; };
	}

	~Phoenix() { delete m_window; }

	void onEvent(const events::Event& e) override
	{
		switch (e.type)
		{
		case events::EventType::KEY_PRESSED:
			switch (e.keyboard.key)
			{
			case events::Keys::KEY_ESCAPE:
				m_camera->enable(!m_camera->isEnabled());
				break;

			case events::Keys::KEY_Q:
				m_window->close();
				break;

			case events::Keys::KEY_E:
				m_playerHand++;
				m_player->setHand(
				    voxels::BlockRegistry::get()->getFromRegistryID(
				        m_playerHand));
				break;

			case events::Keys::KEY_R:
				m_playerHand--;
				m_player->setHand(
				    voxels::BlockRegistry::get()->getFromRegistryID(
				        m_playerHand));
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
				break;

			case events::MouseButtons::RIGHT:
				m_player->action2();
				break;

			default:
				break;
			}

		default:
			break;
		}
	}

	void run()
	{
		voxels::BlockRegistry::get()->initialise();

		{
			voxels::BlockType air;
			{
				air.id       = "core:air";
				air.category = voxels::BlockCategory::AIR;
			}
			voxels::BlockRegistry::get()->registerBlock(air);
		}

		std::string save = "save1";

		if (!ContentManager::get()->loadModules(save))
		{
			m_window->close();
		}

		Settings::get()->load();
		m_world  = new voxels::ChunkManager(3, voxels::Map(save, "map1"));
		m_player = new Player(m_world);
		m_camera = new gfx::FPSCamera(m_window);
		m_camera->setActor(m_player);

		m_player->setHand(voxels::BlockRegistry::get()->getFromRegistryID(0));

		gfx::ShaderPipeline shaderPipeline;
		shaderPipeline.prepare("Assets/SimpleWorld.vert",
		                       "Assets/SimpleWorld.frag",
		                       gfx::ChunkRenderer::getRequiredShaderLayout());

		shaderPipeline.activate();

		const math::mat4 model;
		shaderPipeline.setMatrix("u_model", model);

		static bool       wireframe    = false;
		static bool       followCamera = true;
		static int        prevSens;
		static math::vec3 lastPos;

		m_window->setVSync(true);

		float last = static_cast<float>(SDL_GetTicks());
		while (m_window->isRunning())
		{
			const float now = static_cast<float>(SDL_GetTicks());
			const float dt  = now - last;
			last            = now;

			m_window->startFrame();

			m_camera->tick(dt);
			m_world->tick(m_camera->getPosition());

			{
				ImGuiIO& io         = ImGui::GetIO();
				ImVec2   window_pos = ImVec2(io.DisplaySize.x - 10.f, 10.f);
				ImVec2   window_pos_pivot = ImVec2(1.0f, 0.0f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always,
				                        window_pos_pivot);
				ImGui::SetNextWindowBgAlpha(0.3f);

				if (ImGui::Begin(
				        "Debug Overlay Hint", reinterpret_cast<bool*>(1),
				        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
				            ImGuiWindowFlags_NoResize |
				            ImGuiWindowFlags_AlwaysAutoResize |
				            ImGuiWindowFlags_NoSavedSettings |
				            ImGuiWindowFlags_NoFocusOnAppearing |
				            ImGuiWindowFlags_NoNav))
				{
					ImGui::Text("Press Q to exit");
				}

				ImGui::End();
			}

			ImGui::Begin("Debug View");
			if (ImGui::Checkbox("Wireframe", &wireframe))
			{
				if (wireframe)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			ImGui::Checkbox("Follow Camera", &followCamera);
			if (followCamera)
				lastPos = m_player->getPosition();

			static Setting* sensSetting =
			    Settings::get()->getSetting("camera:sensitivity");
			static int sens = sensSetting->value();
			ImGui::SliderInt("cam sensitivity", &sens, 0, 100);
			if (sens != prevSens)
			{
				prevSens = sens;
				sensSetting->set(sens);
			}

			ImGui::Text("X: %f\nY: %f\nZ: %f", m_player->getPosition().x,
			            m_player->getPosition().y, m_player->getPosition().z);

			ImGui::Text("Block in hand: %i: %s", m_playerHand,
			            m_player->getHand()->displayName.c_str());
			ImGui::End();

			chat.draw();

			shaderPipeline.setMatrix("u_view", m_camera->calculateViewMatrix());
			shaderPipeline.setMatrix("u_projection", m_camera->getProjection());

			m_world->render();

			m_window->endFrame();
		}

		// ============== //
		// Begin Shutdown //
		// ============== //
		Settings::get()->save();

		delete m_world;
		delete m_camera;
		delete m_player;
		delete m_window;
	}

private:
	gfx::Window*          m_window = nullptr;
	gfx::FPSCamera*       m_camera = nullptr;
	Player*               m_player = nullptr;
	voxels::ChunkManager* m_world  = nullptr;
	int                   m_playerHand = 0;
};

#undef main
int main(int argc, char** argv)
{
	Phoenix* game = new Phoenix();
	game->run();

	return 0;
}
