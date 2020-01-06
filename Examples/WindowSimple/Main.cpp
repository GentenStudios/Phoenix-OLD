#include <Quartz2/Events/Event.hpp>
#include <Quartz2/Events/IEventListener.hpp>
#include <Quartz2/Graphics/Window.hpp>
#include <Quartz2/Voxels/BlockRegistry.hpp>

#include <iostream>

class Game : public q2::events::IEventListener
{
public:
	Game()
	{
		m_window = new q2::gfx::Window("Sandbox", 1280, 720);
		m_window->registerEventListener(this);
	}

	~Game() { delete m_window; }

	void onEvent(const q2::events::Event& e) override
	{
		///////////////////////////////////////////////////////////////////////
		// NOTE: Check out the Event struct to find out what else it can do.
		// For example, it can provide positional information (whether it be
		// mouse position, or window position, etc...), sizes (such as for
		// windows), scroll events, keyboard events and mouse events.
		// More documentation will be available soon.
		///////////////////////////////////////////////////////////////////////

		using namespace q2::events;
		if (e.type == EventType::KEY_PRESSED)
		{
			if (e.keyboard.key == Keys::KEY_ESCAPE)
			{
				m_window->close();
			}
		}
	}

	void run()
	{
		while (m_window->isRunning())
		{
			m_window->startFrame();
			m_window->endFrame();
		}
	}

private:
	q2::gfx::Window* m_window;
};

int main(int argc, char* argv[])
{
	Game* app = new Game();
	app->run();
	delete app;

	return 0;
}
