#include <Quartz2/Events/Event.hpp>
#include <Quartz2/Graphics/Window.hpp>
#include <Quartz2/Quartz.hpp>

#include <Sandbox/UI.hpp>

#include <imgui.h>

#include <iostream>
#include <sstream>
#include <memory>
#include <thread>
#include <string>
#include <string_view>

using namespace q2;

static Commander kirk = Commander();

static void commandCenter(const std::string &input, std::ostringstream &cout)
{
	cout << "->" << input << "\n";

	// String views are cheaper and since the push_back vector function
	// copies the contents of the input string as well we can avoid
	// directly copying characters for the most part here.
	std::string_view search = input;
	std::string command;
	std::string arg; // Just used to copy the args out of the search string.
	std::vector<std::string> args;
	size_t searchLoc;
	size_t spaceLoc;

	// Substring was unnecessary because it creates a duplicate string
	// to store the memory in when we can just refference it statically.
	if ( ! search.empty() && search[0] == '/')
	{
		// NOTE:
		//   Can't enter \t or \n rn, might be a good idea to sanitize l8r
		// Search `first_of` initially, it's faster when there's no spaces.
		spaceLoc = search.find_first_of(' ');

		// if we don't have arguments don't try and populate the args array.
		if (spaceLoc != std::string_view::npos)
		{
			command = search.substr(1, spaceLoc);

			// doesn't create a new string object, just moves the start forward.
			search.remove_prefix(1+spaceLoc);

			// `first_not_of` space keeps errors from happening if a user
			// accidentally separates the args with extra whitespace.
			while ((searchLoc = search.find_first_not_of(' ')) != std::string_view::npos)
			{
				search.remove_prefix(searchLoc); // strip the leading whitspace
				spaceLoc = search.find_first_of(' ');
				arg = search.substr(0, (spaceLoc > 0 ? spaceLoc : search.length()));
				args.push_back(arg);
				search.remove_prefix(spaceLoc);
			}
		}
		else
		{
			// otherwise just use the whole string without the command char.
			command = search.substr(1, search.length());
		}

		kirk.run(command, std::move(args), cout);
	}
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
		sol::state lua;
		lua.open_libraries(sol::lib::base);
		luaapi::loadAPI(lua);
		bool loadedLua = modules::loadModules("save1", lua);
		if (!loadedLua)
		{
			m_window->close();
		}

		m_chunkRenderer = std::make_unique<ChunkRenderer>();
		m_chunkRenderer->setup();

		if (!m_chunkRenderer->isReady())
		{
			std::fprintf(stderr, "Renderer setup failed: Status = %s\n",
			             enums::toString(m_chunkRenderer->status()));

			m_window->close();
		}

		std::shared_ptr<BlockTextureAtlas> atlas =
		    std::make_shared<BlockTextureAtlas>(16, 16);
		atlas->addTextureFile("Assets/grass_top.png");
		atlas->addTextureFile("Assets/dirt.png");
		atlas->addTextureFile("Assets/grass_side.png");
		atlas->patch();

		BlockRegistry* blocksRegistery = BlockRegistry::get();
		blocksRegistery->setAtlas(atlas);

		blocksRegistery->registerBlock(
		    {"Air", "core:air", BLOCK_CATEGORY_AIR, {}});

		BlockType* dirtBlockType = blocksRegistery->registerBlock(
		    {"Dirt", "core:dirt", BLOCK_CATEGORY_SOLID, {}});
		BlockType* grassBlockType = blocksRegistery->registerBlock(
		    {"Grass", "core:grass", BLOCK_CATEGORY_SOLID, {}});

		dirtBlockType->textures.setAll(
		    atlas->getSpriteIDFromFilepath("Assets/dirt.png"));

		grassBlockType->textures.top =
		    atlas->getSpriteIDFromFilepath("Assets/grass_top.png");
		grassBlockType->textures.bottom =
		    atlas->getSpriteIDFromFilepath("Assets/dirt.png");
		grassBlockType->textures.front    = grassBlockType->textures.back =
		    grassBlockType->textures.left = grassBlockType->textures.right =
		        atlas->getSpriteIDFromFilepath("Assets/grass_side.png");

		m_chunkRenderer->setTexture(atlas->getPatchedTextureData(),
		                            atlas->getPatchedTextureWidth(),
		                            atlas->getPatchedTextureHeight());

		Chunk a;
		m_chunkRenderer->addMesh(a.generateMesh());

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
				ImGuiIO& io = ImGui::GetIO();
				ImVec2 window_pos = ImVec2(io.DisplaySize.x - 10.f, 10.f);
				ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
				ImGui::SetNextWindowBgAlpha(0.3f);

				if (ImGui::Begin("Debug Overlay Hint", reinterpret_cast<bool*>(1),
				                 ImGuiWindowFlags_NoMove |
				                     ImGuiWindowFlags_NoTitleBar |
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
			static int      sens = sensSetting->value();
			ImGui::SliderInt("cam sensitivity", &sens, 0, 100);
			if (sens != prevSens)
			{
				prevSens = sens;
				sensSetting->set(sens);
			}
			ImGui::End();

			chat.draw();

			m_chunkRenderer->render(m_camera);

			m_window->endFrame();
		}
	}

private:
	gfx::Window* m_window;

	std::unique_ptr<ChunkRenderer> m_chunkRenderer;
	gfx::FPSCamera*                m_camera;
};

#undef main
int main(int argc, char** argv)
{
	Phoenix* game = new Phoenix();
	game->run();

	return 0;
}
