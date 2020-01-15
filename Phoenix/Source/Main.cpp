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

#include <Phoenix/ContentLoader.hpp>
#include <Phoenix/Graphics/Camera.hpp>
#include <Phoenix/Graphics/ChunkMesher.hpp>
#include <Phoenix/Graphics/ChunkRenderer.hpp>
#include <Phoenix/Graphics/Window.hpp>
#include <Phoenix/ImGuiHelpers.hpp>
#include <Phoenix/Settings.hpp>
#include <Phoenix/Voxels/BlockRegistry.hpp>
#include <Phoenix/Voxels/Chunk.hpp>

#include <Phoenix/UI.hpp>

#include <imgui.h>

#include <sstream>
#include <string>
#include <fstream>

using namespace phx;

static void rawEcho(const std::string& input, std::ostringstream& cout)
{
	cout << input << "\n";
}

static ui::ChatWindow chat("Chat Window", 5,
                           "Type something and hit enter to run a command!\n");

// This gross thing is our desire to use std::filesystem despite OSX not
// supporting it until 10.15. Of the methods we tried, this was the easiest and
// most functional solution without importing another library to use intead of
// std::filesystem on all other systems. When Apple stops performing security
// updates for OSX 10.14 (Sometime in 2022?) we will delete this and just use
// the std::filesystem code inline where the function exists. 
// - @sonosfuer (Austin)
#if defined(__cpp_lib_filesystem)
#	include <filesystem>
	void initFiles(std::string save) 
	{ 
		if (!std::filesystem::exists("Save")){
			std::filesystem::create_directory("Save");
		}
		if (!std::filesystem::exists("Save/"+ save)){
			std::filesystem::create_directory("Save/" + save);
			std::ofstream mods;
			mods.open("Save/save1/Mods.txt");
			mods.close();
		}

	}
#else
	void initFiles(std::string save)
	{
		std::ofstream saveFile;
		saveFile.open("Save/" + save + "/mods.txt")
		if(saveFile != null){
			saveFile.close();
			return;
		} else {
			mkdir("Save");
			mkdir("Save/" + save);
			saveFile.open("Save/" + save + "/mods.txt");
			saveFile.close();
			return;
		}
	}
#endif

class Phoenix : public events::IEventListener
{
public:
	Phoenix()
	{
		m_window = new gfx::Window("Phoenix Game!", 1280, 720);
		m_window->registerEventListener(this);

		m_camera = new gfx::FPSCamera(m_window);

		chat.registerCallback(&rawEcho);
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

			default:
				break;
			}
			break;

		default:
			break;
		}
	}

	void run()
	{
		std::string save = "save1";
		initFiles(save);
		sol::state lua;
		lua.open_libraries(sol::lib::base);
		luaapi::loadAPI(lua);
		bool loadedLua = modules::loadModules("save1", lua);
		if (!loadedLua)
		{
			m_window->close();
		}
		{
			using namespace phx::voxels;
			BlockRegistry::get()->initialise();

			BlockType grassBlock;
			{
				grassBlock.displayName = "Grass";
				grassBlock.id          = "core:grass";
				grassBlock.category    = BlockCategory::SOLID;

				// top, left, back, right, top, bottom
				grassBlock.textures = {
				    "Assets/grass_side.png", "Assets/grass_side.png",
				    "Assets/grass_side.png", "Assets/grass_side.png",
				    "Assets/grass_top.png",  "Assets/dirt.png",
				};
			}

			BlockRegistry::get()->registerBlock(grassBlock);
		}

		phx::gfx::ChunkRenderer renderer(100);
		renderer.buildTextureArray();

		for (int j = 0; j < 10; ++j)
		{
			for (int i = 0; i < 10; ++i)
			{
				phx::voxels::Chunk chunk({i * 16, 0, j * 16});
				chunk.autoTestFill();
				phx::gfx::ChunkMesher mesher(chunk.getChunkPos(),
				                             chunk.getBlocks(),
				                             renderer.getTextureTable());
				mesher.mesh();
				renderer.submitChunkMesh(mesher.getMesh(), i + (j * 10));
			}
		}

		phx::gfx::ShaderPipeline shaderPipeline;
		shaderPipeline.prepare("Assets/SimpleWorld.vert",
		                       "Assets/SimpleWorld.frag",
		                       renderer.getRequiredShaderLayout());

		shaderPipeline.activate();

		phx::math::mat4 model;
		shaderPipeline.setMatrix("u_model", model);

		static bool wireframe = false;
		static int  prevSens;

		float last = static_cast<float>(SDL_GetTicks());
		while (m_window->isRunning())
		{
			const float now = static_cast<float>(SDL_GetTicks());
			const float dt  = now - last;
			last            = now;

			m_window->startFrame();

			m_camera->tick(dt);

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

			static Setting* sensSetting =
			    Settings::get()->getSetting("camera:sensitivity");
			static int sens = sensSetting->value();
			ImGui::SliderInt("cam sensitivity", &sens, 0, 100);
			if (sens != prevSens)
			{
				prevSens = sens;
				sensSetting->set(sens);
			}
			ImGui::End();

			chat.draw();

			shaderPipeline.setMatrix("u_view", m_camera->calculateViewMatrix());
			shaderPipeline.setMatrix("u_projection", m_camera->getProjection());

			renderer.render();

			m_window->endFrame();
		}
	}

private:
	gfx::Window*    m_window;
	gfx::FPSCamera* m_camera;
};

#undef main
int main(int argc, char** argv)
{
	Phoenix* game = new Phoenix();
	game->run();

	return 0;
}
