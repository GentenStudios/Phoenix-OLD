#include <Quartz2/Events/Event.hpp>
#include <Quartz2/Quartz.hpp>
#include <Sandbox/UI.hpp>

#include <imgui.h>

#include <iostream>
#include <memory>
#include <thread>
#include <sstream>
#include <string>

using namespace q2;

static Commander kirk = Commander();

static void rawEcho(const std::string &input, std::ostringstream &cout)
{
	// easter egg commission for the tobster.
	if (input.compare("buh-buh-bum-bah-bum") == 0)
		cout << "I'll be back\n";

	cout << "->" << input << "\n";

	std::string s = input;

	if (s.substr(0,1) == "/") {
		std::vector<std::string> args;

		size_t pos = s.find(" ");
		std::string command = s.substr(1, pos);
		s.erase(0, pos + 1);
		std::string token;
		while ((pos = s.find(" ")) != std::string::npos) {
			token = s.substr(0, pos);
			args.push_back(token);
			s.erase(0, pos + 1);
		}
		kirk.run(command, std::move(args), cout);
	}
}

static ui::ChatWindow chat(
    "Chat Window", 5,
    // @FutureRuby I know this looks weird but it works.
    // constant string compile time concatenation is wierd.
    "Welcome to the Darklight Terminal!\n"
    "Type something and hit enter to run a command!\n");

static ImGui::BasicTerminal term("Test Terminal", 5);

class PhoenixGame : public Game
{
private:
	std::unique_ptr<ChunkRenderer> m_chunkRenderer;
	std::unique_ptr<Camera>        m_camera;

public:
	PhoenixGame() : Game(1280, 720, "Project Phoenix!")
	{

		chat.registerCallback(&rawEcho);
		term.registerCallback(&rawEcho);
	}

protected:
	virtual bool onEvent(SDL_Event e)
	{
		if (e.type == SDL_KEYUP)
		{
			if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				m_camera->enable(!m_camera->isEnabled());
				SDL_ShowCursor(!m_camera->isEnabled());
				return true;
			}
			// Uncomment to have chat popup every time enter is pressed.
			// Could probably use integration with @SonosFuer's input system.
			// if (e.key.keysym.scancode == SDL_SCANCODE_RETURN)
			// NOTE:
			//   doing this encurs the wrath of cthulu and you will be cursed to
			//   oblivion. By which I mean you won't be able to press enter
			//   without the chat window being focused on period, regardless of
			//   other ui elements being overlayed. I tryed to make this a
			//   non-issue by by leaving the ImGuiWindowFlags_NoFocusOnAppearing
			//   flag with the class definition but it can be a pain when using
			//   a second terminal.
			//
			// {
			// 	chat.focus();
			// 	return true;
			// }
		}

		return false;
	}

	virtual void onStart()
	{
		m_chunkRenderer = std::make_unique<ChunkRenderer>();
		m_chunkRenderer->setup(getWindowWidth(), getWindowHeight());

		if (!m_chunkRenderer->isReady())
		{
			std::fprintf(stderr, "Renderer setup failed: Status = %s\n",
			             enums::toString(m_chunkRenderer->status()));

			exitGame();
		}

		m_camera = std::make_unique<Camera>();
		SDL_ShowCursor(0);

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

	}

	virtual void onExit() { m_chunkRenderer->teardown(); }

	void showDebugStats(int fps, float dt)
	{
		const float DISTANCE = 10.0f;
		const int   corner   = 1;
		static bool p_open   = true;

		ImGuiIO& io = ImGui::GetIO();

		ImVec2 window_pos =
		    ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE,
		           (corner & 2) ? io.DisplaySize.y - DISTANCE * 2.5f
		                        : DISTANCE * 2.5f);
		ImVec2 window_pos_pivot =
		    ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

		ImGui::SetNextWindowBgAlpha(0.3f);

		ImGui::Begin(
		    "Debug Overlay Hint", &p_open,
		    (corner != -1 ? ImGuiWindowFlags_NoMove : 0) |
		        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		        ImGuiWindowFlags_AlwaysAutoResize |
		        ImGuiWindowFlags_NoSavedSettings |
		        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

		ImGui::Text("FPS %i\n", static_cast<int>(fps));
		ImGui::Text("Delta time: %f\n", dt);

		ImGui::End();
	}

	virtual void onFrame(float dt)
	{
		static bool wireframe = false;

		m_camera->tick(dt, getSDLWindow());

		OpenGL32::clearScreen(0.2196f, 0.2196f, 0.2196f, 1.f);

		showDebugStats(getFPS(), dt);

		ImGui::Begin("Debug View");
		if (ImGui::Checkbox("Wireframe", &wireframe))
		{
			if (wireframe)
				OpenGL32::enableWireframe();
			else
				OpenGL32::disableWireframe();
		}
		ImGui::Text("OpenGL Error %i\n", glGetError());
		ImGui::End();

		// Vertical height adjust the chat window... happens in the UI
		// segment definition because Main function management is hard enough
		// as it is. Here's to the good time of whomever doesn't have to deal
		// with 20 extra lines of clutter because of me :D
		chat.draw();
		term.draw();

		ImGui::ShowStyleEditor();
		m_chunkRenderer->render(m_camera.get());
	}
};

#undef main
int main(int argc, char** argv)
{
	Game* game = new PhoenixGame();
	game->start();

	return 0;
}
