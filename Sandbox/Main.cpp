#include <Quartz2/Events/Event.hpp>
#include <Quartz2/Events/IEventListener.hpp>
#include <Quartz2/Graphics/Camera.hpp>
#include <Quartz2/Graphics/ChunkMesher.hpp>
#include <Quartz2/Graphics/ChunkRenderer.hpp>
#include <Quartz2/Graphics/ShaderPipeline.hpp>
#include <Quartz2/Graphics/Window.hpp>

#include <Quartz2/FileIO.hpp>
#include <Quartz2/Voxels/BlockRegistry.hpp>
#include <Quartz2/Voxels/Chunk.hpp>

#include <glad/glad.h>

#include <fstream>
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

			m_camera = new q2::gfx::FPSCamera(m_window);
			m_camera->enable(true);
		}

		~Game() { delete m_window; }

		void onEvent(const q2::events::Event& e) override
		{
			using namespace q2::events;
			if (e.type == EventType::KEY_PRESSED)
			{
				if (e.keyboard.key == Keys::KEY_ESCAPE)
				{
					m_camera->enable(!m_camera->isEnabled());
				}

				if (e.keyboard.key == Keys::KEY_0)
				{
					m_window->close();
				}
			}
			else if (e.type == EventType::WINDOW_RESIZED)
			{
				std::cout << "Window Resized" << std::endl;
			}
			else if (e.type == EventType::WINDOW_MAXIMIZED)
			{
				std::cout << "Window Maximised" << std::endl;
			}
		}

		void run()
		{
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
					    "Assets/grass_side.png", "Assets/grass_side.png", "Assets/grass_side.png",
					    "Assets/grass_side.png", "Assets/grass_top.png", "Assets/dirt.png",
					};
				}

				BlockRegistry::instance()->registerBlock(grassBlock);
			}

			q2::gfx::ChunkRenderer renderer(100);
			renderer.buildTextureArray();

			for (int j = 0; j < 10; ++j)
			{
				for (int i = 0; i < 10; ++i)
				{
					q2::voxels::Chunk chunk({i * 16, 0, j * 16});
					chunk.autoTestFill();
					q2::gfx::ChunkMesher mesher(chunk.getChunkPos(),
					                            chunk.getBlocks(),
					                            renderer.getTextureTable());
					mesher.mesh();
					renderer.submitChunkMesh(mesher.getMesh(), i + (j * 10));
				}
			}

			q2::gfx::ShaderPipeline shaderPipeline;
			shaderPipeline.prepare("Assets/SimpleWorld.vert",
			                       "Assets/SimpleWorld.frag",
			                       renderer.getRequiredShaderLayout());

			shaderPipeline.activate();

			q2::math::mat4 model;
			shaderPipeline.setMatrix("u_model", model);

			float last = static_cast<float>(SDL_GetTicks());
			while (m_window->isRunning())
			{
				const float now = static_cast<float>(SDL_GetTicks());
				const float dt  = now - last;
				last            = now;

				m_camera->tick(dt);

				m_window->startFrame();

				shaderPipeline.setMatrix("u_projection",
				                         m_camera->getProjection());
				shaderPipeline.setMatrix("u_view",
				                         m_camera->calculateViewMatrix());

				renderer.render();

				m_window->endFrame();
			}
		}

	private:
		q2::gfx::Window*    m_window;
		q2::gfx::FPSCamera* m_camera;
	};
} // namespace sandbox

// SDL defines main like the poopoo it is.
#undef main
int main(int argc, char* argv[])
{
	sandbox::Game* app = new sandbox::Game();
	app->run();
	delete app;

	return 0;
}
