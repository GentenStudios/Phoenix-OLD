#include <Quartz2/Events/Event.hpp>
#include <Quartz2/Events/IEventListener.hpp>
#include <Quartz2/Graphics/Window.hpp>
#include <Quartz2/Voxels/BlockRegistry.hpp>

#include <iostream>

namespace sandbox
{
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
} // namespace sandbox

// SDL defines main like the poopoo it is.
#undef main
int main(int argc, char* argv[])
{
	using namespace q2::voxels;

	BlockRegistry::instance()->initialise();

	BlockType grassBlock;
	{
		grassBlock.displayName = "Grass";
		grassBlock.id          = "core:grass";
		grassBlock.category    = BlockCategory::SOLID;

		// top, left, back, right, top, bottom
		grassBlock.textures = {
		    "grass.png", "dirt.png", "dirt.png",
		    "dirt.png",  "dirt.png", "dirt.png",
		};
	}

	BlockType dirtBlock;
	{
		dirtBlock.displayName = "Dirt";
		dirtBlock.id          = "core:dirt";
		dirtBlock.category    = BlockCategory::SOLID;

		//// top, left, back, right, top, bottom
		// dirtBlock.textures = {"dirt.png", "dirt.png", "dirt.png",
		//                       "dirt.png", "dirt.png", "dirt.png"};
		// OH WAIT, WE DON'T NEED TO DO THIS BIT LIKE THIS, WE CAN DO IT LIKE
		// THIS:
		dirtBlock.setAllTextures("dirt.png");
	}

	BlockRegistry::instance()->registerBlock(grassBlock);
	BlockRegistry::instance()->registerBlock(dirtBlock);

	TextureRegistry* registeredTextures =
	    BlockRegistry::instance()->getTextures();

	auto textures = registeredTextures->getTextures();

	for (auto texture : textures)
	{
		std::cout << "Texture: " << texture << std::endl;
	}

	sandbox::Game* app = new sandbox::Game();
	app->run();
	delete app;

	return 0;
}
