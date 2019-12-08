#include <Quartz2/Game.hpp>
#include <Quartz2/ChunkRenderer.hpp>

#include <memory>

using namespace q2;

class PhoenixGame : public Game
{
private:
	std::unique_ptr<ChunkRenderer> m_chunkRenderer;

public:
	PhoenixGame()
		: Game(800, 600, "Phoenix Game!")
	{}

protected:
	virtual void onStart()
	{
		m_chunkRenderer = std::make_unique<ChunkRenderer>();
		m_chunkRenderer->setup();
	
		if (!m_chunkRenderer->isReady())
		{
			std::fprintf(stderr, "Renderer setup failed: Status = %s\n",
				enums::toString(m_chunkRenderer->status()));

			exitGame();
		}
	}
	
	virtual void onExit()
	{
		m_chunkRenderer->teardown();
	}

	virtual void onFrame()
	{
		OpenGL32::clearScreen(0.f, 0.f, 0.f, 0.f);

		m_chunkRenderer->render();
	}
};

int main(int argc, char** argv)
{
	Game* game = new PhoenixGame();
	game->start();

	return 0;
}