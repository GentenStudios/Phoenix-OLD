// Copyright 2019 Genten Studios
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <Quartz2/Game.hpp>
#include <Quartz2/OpenGL32.hpp>

#include <SDL_opengl.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

using namespace q2;

Game::Game(std::size_t windowWidth, std::size_t windowHeight, const std::string& windowTitle)
	: m_windowWidth(windowWidth), m_windowHeight(windowHeight), m_windowTitle(windowTitle),
	m_sdlWindow(NULL), m_sdlGLContext()
{
}

void Game::exitGame()
{
	SDL_Event e;
	e.type = SDL_QUIT;
	e.quit.timestamp = SDL_GetTicks();
	e.quit.type  = SDL_QUIT;

	SDL_PushEvent(&e);
}

void Game::start()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	m_sdlWindow = SDL_CreateWindow(m_windowTitle.c_str(), (int)SDL_WINDOWPOS_CENTERED, (int)SDL_WINDOWPOS_CENTERED,
		m_windowWidth, m_windowHeight, SDL_WINDOW_OPENGL);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	m_sdlGLContext = SDL_GL_CreateContext(m_sdlWindow);

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& imguiIO = ImGui::GetIO();
	imguiIO.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	ImGui_ImplSDL2_InitForOpenGL(m_sdlWindow, m_sdlGLContext);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	onStart();

	int lastTime = SDL_GetTicks();

	bool running = true;
	while (running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			if (!onEvent(e))
			{
				switch (e.type)
				{
				case SDL_QUIT:
					running = false;
					break;
				}
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(m_sdlWindow);
		ImGui::NewFrame();

		const int currentTime = SDL_GetTicks();
		const float dt = static_cast<float>(currentTime - lastTime);

		onFrame(dt);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(m_sdlWindow);

		lastTime = currentTime;
	}

	onExit();
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyWindow(m_sdlWindow);
	SDL_Quit();
}