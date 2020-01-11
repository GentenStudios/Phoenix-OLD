#include <Quartz2/Events/Event.hpp>
#include <Quartz2/Graphics/Window.hpp>
#include <Quartz2/Quartz.hpp>

#include <Sandbox/UI.hpp>

#include <imgui.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using namespace q2;

static void rawEcho(const std::string& input, std::ostringstream& cout)
{
	cout << input << "\n";
}

static ui::ChatWindow chat(
    "Chat Window", 5,
    // @FutureRuby I know this looks weird but it works.
    // constant string compile time concatenation is wierd.
    "Welcome to the Darklight Terminal!\n"
    "Type something and hit enter to run a command!\n");

class Phoenix : public events::IEventListener
{
public:
	Phoenix()
	{
		m_window = new gfx::Window("Phoenix Game!", 1280, 720);
		m_window->registerEventListener(this);
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
		while (m_window->isRunning())
		{
			m_window->startFrame();

			ImGui::ShowDemoWindow();
			
			m_window->endFrame();
		}
	}

private:
	gfx::Window* m_window;
};

#undef main
int main(int argc, char** argv)
{
	Phoenix* game = new Phoenix();
	game->run();

	return 0;
}

//		sol::state lua;
// lua.open_libraries(sol::lib::base);
// luaapi::loadAPI(lua);
// bool loadedLua = modules::loadModules("save1", lua);
// if (!loadedLua)
//{
//	exitGame();
//}
//
// m_chunkRenderer = std::make_unique<ChunkRenderer>();
// m_chunkRenderer->setup(getWindowWidth(), getWindowHeight());
//
// if (!m_chunkRenderer->isReady())
//{
//	std::fprintf(stderr, "Renderer setup failed: Status = %s\n",
//	             enums::toString(m_chunkRenderer->status()));
//
//	exitGame();
//}
//
// m_camera = std::make_unique<Camera>();
// SDL_ShowCursor(0);
//
// std::shared_ptr<BlockTextureAtlas> atlas =
//    std::make_shared<BlockTextureAtlas>(16, 16);
// atlas->addTextureFile("Assets/grass_top.png");
// atlas->addTextureFile("Assets/dirt.png");
// atlas->addTextureFile("Assets/grass_side.png");
// atlas->patch();
//
// BlockRegistry* blocksRegistery = BlockRegistry::get();
// blocksRegistery->setAtlas(atlas);
//
// blocksRegistery->registerBlock({"Air", "core:air", BLOCK_CATEGORY_AIR, {}});
//
// BlockType* dirtBlockType = blocksRegistery->registerBlock(
//    {"Dirt", "core:dirt", BLOCK_CATEGORY_SOLID, {}});
// BlockType* grassBlockType = blocksRegistery->registerBlock(
//    {"Grass", "core:grass", BLOCK_CATEGORY_SOLID, {}});
//
// dirtBlockType->textures.setAll(
//    atlas->getSpriteIDFromFilepath("Assets/dirt.png"));
//
// grassBlockType->textures.top =
//    atlas->getSpriteIDFromFilepath("Assets/grass_top.png");
// grassBlockType->textures.bottom =
//    atlas->getSpriteIDFromFilepath("Assets/dirt.png");
// grassBlockType->textures.front    = grassBlockType->textures.back =
//    grassBlockType->textures.left = grassBlockType->textures.right =
//        atlas->getSpriteIDFromFilepath("Assets/grass_side.png");
//
// m_chunkRenderer->setTexture(atlas->getPatchedTextureData(),
//                            atlas->getPatchedTextureWidth(),
//                            atlas->getPatchedTextureHeight());
//
// Chunk a;
// m_chunkRenderer->addMesh(a.generateMesh());
