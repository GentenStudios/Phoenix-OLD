#include <Quartz2/Events/Event.hpp>
#include <Quartz2/Events/IEventListener.hpp>
#include <Quartz2/Graphics/Camera.hpp>
#include <Quartz2/Graphics/Window.hpp>

#include <glad/glad.h>

#include <fstream>
#include <iostream>

std::string readAllFile(const std::string& filepath)
{
	std::fstream fileHandle;
	fileHandle.open(filepath.c_str());

	std::string fileString;
	fileString.assign((std::istreambuf_iterator<char>(fileHandle)),
	                  (std::istreambuf_iterator<char>()));

	fileHandle.close();

	return fileString;
}
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
			// clang-format off
		    float vertices[] = {
		        -0.5f, -0.5f, -0.5f,
		         0.5f, -0.5f, -0.5f,
		         0.5f,  0.5f, -0.5f,
		         0.5f,  0.5f, -0.5f,
		        -0.5f,  0.5f, -0.5f,
		        -0.5f, -0.5f, -0.5f,

		        -0.5f, -0.5f,  0.5f,
		         0.5f, -0.5f,  0.5f,
		         0.5f,  0.5f,  0.5f,
		         0.5f,  0.5f,  0.5f,
		        -0.5f,  0.5f,  0.5f,
		        -0.5f, -0.5f,  0.5f,

		        -0.5f,  0.5f,  0.5f,
		        -0.5f,  0.5f, -0.5f,
		        -0.5f, -0.5f, -0.5f,
		        -0.5f, -0.5f, -0.5f,
		        -0.5f, -0.5f,  0.5f,
		        -0.5f,  0.5f,  0.5f,

		         0.5f,  0.5f,  0.5f,
		         0.5f,  0.5f, -0.5f,
		         0.5f, -0.5f, -0.5f,
		         0.5f, -0.5f, -0.5f,
		         0.5f, -0.5f,  0.5f,
		         0.5f,  0.5f,  0.5f,

		        -0.5f, -0.5f, -0.5f,
		         0.5f, -0.5f, -0.5f,
		         0.5f, -0.5f,  0.5f,
		         0.5f, -0.5f,  0.5f,
		        -0.5f, -0.5f,  0.5f,
		        -0.5f, -0.5f, -0.5f,

		        -0.5f,  0.5f, -0.5f,
		         0.5f,  0.5f, -0.5f,
		         0.5f,  0.5f,  0.5f,
		         0.5f,  0.5f,  0.5f,
		        -0.5f,  0.5f,  0.5f,
		        -0.5f,  0.5f, -0.5f
		    };
			// clang-format on
			
			unsigned int VBO, VAO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
			             GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			                      nullptr);
			glEnableVertexAttribArray(0);

			std::string vertexShaderSource =
			    readAllFile("Assets/SimpleWorld.vert");

			std::string fragmentShaderSource =
			    readAllFile("Assets/SimpleWorld.frag");

			const char* vertexShaderSourceREAL = vertexShaderSource.c_str();
			const char* fragmentShaderSourceREAL = fragmentShaderSource.c_str();

			int vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &vertexShaderSourceREAL, nullptr);
			glCompileShader(vertexShader);

			// check for shader compile errors
			int  success;
			char infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				          << infoLog << std::endl;
			}

			// fragment shader
			int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &fragmentShaderSourceREAL, nullptr);
			glCompileShader(fragmentShader);

			// check for shader compile errors
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				          << infoLog << std::endl;
			}

			// link shaders
			int shaderProgram = glCreateProgram();
			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);

			// check for linking errors
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				          << infoLog << std::endl;
			}

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			glUseProgram(shaderProgram);

			q2::math::mat4 model;
			unsigned int   modelLoc =
			    glGetUniformLocation(shaderProgram, "u_model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model.elements[0]);

			unsigned int viewLoc =
			    glGetUniformLocation(shaderProgram, "u_view");
			unsigned int projLoc =
			    glGetUniformLocation(shaderProgram, "u_projection");

			float last = static_cast<float>(SDL_GetTicks());
			while (m_window->isRunning())
			{
				const float now = static_cast<float>(SDL_GetTicks());
				const float dt  = now - last;
				last            = now;

				m_camera->tick(dt);

				m_window->startFrame();
				glUniformMatrix4fv(projLoc, 1, GL_FALSE,
				                   m_camera->getProjection().elements);
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE,
				                   m_camera->calculateViewMatrix().elements);
				glDrawArrays(GL_TRIANGLES, 0, 36);
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
