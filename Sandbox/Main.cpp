#include <Quartz2/Quartz.hpp>

#include <imgui.h>

#include <memory>

using namespace q2;

class PhoenixGame : public Game
{
private:
	std::unique_ptr<ChunkRenderer> m_chunkRenderer;
	std::unique_ptr<Camera> m_camera;

public:
	PhoenixGame()
		: Game(1280, 720, "Project Phoenix!")
	{}

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
		}

		return false;
	}

	virtual void onStart()
	{
		sol::state lua;
		lua.open_libraries(sol::lib::base);
		luaapi::loadAPI(lua);
		bool loadedLua = modules::loadModules("save1", lua);
		if (!loadedLua){
			exitGame();
		}

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

		std::shared_ptr<BlockTextureAtlas>atlas = std::make_shared<BlockTextureAtlas>(16, 16);
		atlas->addTextureFile("Assets/grass_top.png");
		atlas->addTextureFile("Assets/dirt.png");
		atlas->addTextureFile("Assets/grass_side.png");
		atlas->patch();

		BlockRegistry* blocksRegistery = BlockRegistry::get();
		blocksRegistery->setAtlas(atlas);

		blocksRegistery->registerBlock({ "Air", "core:air", BLOCK_CATEGORY_AIR, {} });

		BlockType* dirtBlockType = blocksRegistery->registerBlock({ "Dirt", "core:dirt", BLOCK_CATEGORY_SOLID, {} });
		BlockType* grassBlockType = blocksRegistery->registerBlock({ "Grass", "core:grass", BLOCK_CATEGORY_SOLID, {} });

		dirtBlockType->textures.setAll(atlas->getSpriteIDFromFilepath("Assets/dirt.png"));

		grassBlockType->textures.top = atlas->getSpriteIDFromFilepath("Assets/grass_top.png");
		grassBlockType->textures.bottom = atlas->getSpriteIDFromFilepath("Assets/dirt.png");
		grassBlockType->textures.front = grassBlockType->textures.back
			= grassBlockType->textures.left = grassBlockType->textures.right =
				atlas->getSpriteIDFromFilepath("Assets/grass_side.png");
		
		m_chunkRenderer->setTexture(atlas->getPatchedTextureData(), atlas->getPatchedTextureWidth(), atlas->getPatchedTextureHeight());

		Chunk a;
		m_chunkRenderer->addMesh(a.generateMesh());
	}
	
	virtual void onExit()
	{
		m_chunkRenderer->teardown();
	}

	void showDebugStats(int fps, float dt)
	{
		const float DISTANCE = 10.0f;
		const int   corner = 1;
		static bool p_open = true;

		ImGuiIO& io = ImGui::GetIO();

		ImVec2 window_pos =
			ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE,
			(corner & 2) ? io.DisplaySize.y - DISTANCE * 2.5f : DISTANCE * 2.5f);
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
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav
		);

		ImGui::Text("FPS %i\n", (int)fps);
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

		m_chunkRenderer->render(m_camera.get());
	}
};

int main(int argc, char** argv)
{
	Game* game = new PhoenixGame();
	game->start();

	return 0;
}