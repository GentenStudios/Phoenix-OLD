#include <Phoenix/Client.hpp>
#include <Phoenix/Commander.hpp>
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

using namespace phx;

Commander kirk = Commander();

static void rawEcho(const std::string& input, std::ostringstream& outstream)
{
	kirk.callback(input, outstream);
}

static ui::ChatWindow chat {"Chat Window", 5,
                            "Type something and hit enter to run a command."};

Phoenix::Phoenix()
    : m_window {new gfx::Window("Phoenix Game!", 1280, 720)},
      m_camera {new gfx::FPSCamera(m_window)}
{
	m_window->registerEventListener(this);
	chat.registerCallback(&rawEcho);
}

Phoenix::~Phoenix()
{
	delete m_window;
	delete m_camera;
}

void Phoenix::onEvent(const events::Event& event)
{
	switch (event.type)
	{
	case events::EventType::KEY_PRESSED:
		switch (event.keyboard.key)
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

	case events::EventType::MOUSE_BUTTON_PRESSED:
		switch (event.mouse.button)
		{
		case events::MouseButtons::LEFT:
			std::cout << "Left mouse button.\n";
			break;

		default:
			break;
		}

	default:
		break;
	}
}

void Phoenix::run()
{
	voxels::BlockRegistry::get()->initialise();

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	ContentManager::loadAPI(lua, chat);
	bool loadedLua {ContentManager::loadModules("save1", lua)};
	if (!loadedLua)
	{
		m_window->close();
	}

	Settings::get()->load();

	gfx::ChunkRenderer renderer {100};
	renderer.buildTextureArray();

	for (int j {0}; j < 10; ++j)
	{
		for (int i {0}; i < 10; ++i)
		{
			voxels::Chunk chunk({i * 16, 0, j * 16});
			chunk.autoTestFill();
			gfx::ChunkMesher mesher {chunk.getChunkPos(), chunk.getBlocks(),
			                         renderer.getTextureTable()};
			mesher.mesh();
			renderer.submitChunkMesh(mesher.getMesh(), i + (j * 10));
		}
	}

	gfx::ShaderPipeline shaderPipeline;
	shaderPipeline.prepare("Assets/SimpleWorld.vert", "Assets/SimpleWorld.frag",
	                       renderer.getRequiredShaderLayout());

	shaderPipeline.activate();

	math::mat4 model;
	shaderPipeline.setMatrix("u_model", model);

	static bool wireframe {false};
	static int  prevSens;

	float last {static_cast<float>(SDL_GetTicks())};
	while (m_window->isRunning())
	{
		const float now {static_cast<float>(SDL_GetTicks())};
		const float dt {now - last};
		last = now;

		m_window->startFrame();

		m_camera->tick(dt);

		ImGuiIO& io {ImGui::GetIO()};
		ImVec2   window_pos = ImVec2(io.DisplaySize.x - 10.f, 10.f);
		ImVec2   window_pos_pivot {1.0f, 0.0f};
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowBgAlpha(0.3f);

		if (ImGui::Begin("Debug Overlay Hint", reinterpret_cast<bool*>(1),
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

		ImGui::Begin("Debug View");
		if (ImGui::Checkbox("Wireframe", &wireframe))
		{
			if (wireframe)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		static Setting* sensSetting {
		    Settings::get()->getSetting("camera:sensitivity")};
		static int sens {sensSetting->value()};
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
	// ============== //
	// Begin Shutdown //
	// ============== //
	Settings::get()->save();
}
