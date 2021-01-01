// Copyright 2019-20 Genten Studios
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

#include <Client/Renderer/Window.hpp>
#include <Client/Graphics/OpenGLTools.hpp>

#include <Common/Logger.hpp>

#include <SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include <algorithm>

using namespace phx::render;

Window::Window(const std::string& title, const math::vec2& size, bool fullscreen)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
	                    SDL_GL_CONTEXT_PROFILE_CORE);

#	ifdef ENGINE_DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#	endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Uint32 is an SDL type.
	Uint32 flags = SDL_WINDOW_OPENGL;
	if (fullscreen)
	{
		// if size == 0, then we want to just use the current desktop
		// resolution.
		if (size == 0)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		else
		{
			// otherwise we do fullscreen with the size they provide.
			flags |= SDL_WINDOW_FULLSCREEN;
		}
	}
	else
	{
		flags |= SDL_WINDOW_RESIZABLE;
	}
	
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, flags);

	if (m_window == nullptr)
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);

		LOG_FATAL("GRAPHICS")
		    << "An error occured while creating a window: " << SDL_GetError();

		m_running = false;

		return;
	}

	m_running = true;

	m_context = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_context);

	if (!gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
	{
		LOG_FATAL("GRAPHICS") << "An error occured while creating the window. (Error in: gladLoadGLLoader)";

		m_running = false;

		return;
	}

#ifdef ENGINE_DEBUG
	GLint glFlags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &glFlags);
	if (glFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(gfx::glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
		                      nullptr, GL_TRUE);
	}
#	endif

	// enable things we know the whole program is going to use.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, size.x, size.y);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& imguiIO = ImGui::GetIO();
	imguiIO.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	// setup InputState
	m_inputState.m_window = m_window;
}

Window::~Window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

const InputState* Window::getInputState() const { return &m_inputState; }

bool Window::isRunning() const { return m_running; }

void Window::close() { m_running = false; }

void Window::swapBuffers() { SDL_GL_SwapWindow(m_window); }

void Window::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event) > 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		ImGuiIO& io = ImGui::GetIO();

		using namespace events;
		Event e;

		switch (event.type)
		{
		case SDL_QUIT:
			m_running = false;
			e.type = EventType::WINDOW_CLOSED;
			dispatchToListeners(e);
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (io.WantCaptureMouse)
				break;
			e.type         = EventType::MOUSE_BUTTON_PRESSED;
			e.mouse.button = static_cast<MouseButtons>(event.button.button);
			e.mouse.x      = event.button.x;
			e.mouse.y      = event.button.y;
			e.mouse.mods   = static_cast<Mods>(SDL_GetModState());
			dispatchToListeners(e);
			break;

		case SDL_MOUSEBUTTONUP:
			if (io.WantCaptureMouse)
				break;
			e.type         = EventType::MOUSE_BUTTON_RELEASED;
			e.mouse.button = static_cast<MouseButtons>(event.button.button);
			e.mouse.mods   = static_cast<Mods>(SDL_GetModState());
			dispatchToListeners(e);
			break;

		case SDL_MOUSEMOTION:
			if (io.WantCaptureMouse)
				break;
			e.type       = EventType::CURSOR_MOVED;
			if (SDL_GetRelativeMouseMode())
			{
				e.position.x = event.motion.xrel;
				e.position.y = event.motion.yrel;
			}
			else
			{
				e.position.x = event.motion.x;
				e.position.y = event.motion.yrel;
			}
			dispatchToListeners(e);
			break;

		case SDL_KEYDOWN:
			if (io.WantCaptureKeyboard)
			{
				break;
			}
			e.type          = EventType::KEY_PRESSED;
			e.keyboard.key  = static_cast<Keys>(event.key.keysym.scancode);
			e.keyboard.mods = static_cast<Mods>(
			    event.key.keysym.mod); // access these with bitwise operators
			                           // like AND (&) and OR (|)
			dispatchToListeners(e);
			break;

		case SDL_KEYUP:
			if (io.WantCaptureKeyboard)
			{
				break;
			}
			e.type          = EventType::KEY_RELEASED;
			e.keyboard.key  = static_cast<Keys>(event.key.keysym.scancode);
			e.keyboard.mods = static_cast<Mods>(
			    event.key.keysym.mod); // access these with bitwise operators
			                           // like AND (&) and OR (|)
			dispatchToListeners(e);
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
				//			case SDL_WINDOWEVENT_RESIZED: <- we don't want this
				//-> https://wiki.libsdl.org/SDL_WindowEventID
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				e.type        = EventType::WINDOW_RESIZED;
				e.size.width  = event.window.data1;
				e.size.height = event.window.data2;
				glViewport(0, 0, e.size.width, e.size.height);
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_FOCUS_GAINED:
				e.type = EventType::WINDOW_FOCUSED;
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_FOCUS_LOST:
				e.type = EventType::WINDOW_DEFOCUSED;
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_CLOSE:
				e.type = EventType::WINDOW_CLOSED;
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_MINIMIZED:
				e.type = EventType::WINDOW_MINIMIZED;
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_MAXIMIZED:
				e.type = EventType::WINDOW_MAXIMIZED;
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_RESTORED:
				e.type = EventType::WINDOW_RESTORED;
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_LEAVE:
				e.type = EventType::CURSOR_LEFT;
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_ENTER:
				e.type = EventType::CURSOR_ENTERED;
				dispatchToListeners(e);
				break;

			case SDL_WINDOWEVENT_MOVED:
				e.type       = EventType::WINDOW_MOVED;
				e.position.x = event.window.data1;
				e.position.y = event.window.data2;
				dispatchToListeners(e);
				break;
			default:
				break;
			}
			break;
		default:
			// if none of above, just ignore.
			break;
		}
	}
}

void Window::registerEventListener(events::IEventListener* listener)
{
	const auto it =
	    std::find(m_eventListeners.begin(), m_eventListeners.end(), listener);
	if (it == m_eventListeners.end())
	{
		m_eventListeners.push_back(listener);
	}
}

void Window::removeEventListener(events::IEventListener* listener)
{
	const auto it =
	    std::find(m_eventListeners.begin(), m_eventListeners.end(), listener);
	if (it != m_eventListeners.end())
	{
		m_eventListeners.erase(it);
	}
}

void Window::show() { SDL_ShowWindow(m_window); }

void Window::hide() { SDL_HideWindow(m_window); }

void Window::maximise() { SDL_MaximizeWindow(m_window); }

void Window::minimise() { SDL_MinimizeWindow(m_window); }

void Window::focus() { SDL_SetWindowInputFocus(m_window); }

std::string Window::getTitle() const
{
	return SDL_GetWindowTitle(m_window);
}

void Window::setTitle(const std::string& title)
{
	SDL_SetWindowTitle(m_window, title.c_str());
}

bool Window::isFullscreen() const
{
	return SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN;
}

void Window::setFullscreen(bool enable)
{
	SDL_SetWindowFullscreen(m_window, enable ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_FALSE);
}

phx::math::vec2 Window::getSize() const
{
	int x, y;
	SDL_GetWindowSize(m_window, &x, &y);
	return {x, y};
}

void Window::resize(const math::vec2& size)
{
	SDL_SetWindowSize(m_window, size.x, size.y);
}

void Window::setResizable(bool enable)
{
	SDL_SetWindowResizable(m_window, enable ? SDL_TRUE : SDL_FALSE);
}

bool Window::isVsync() const { return SDL_GL_GetSwapInterval() != 0; }

void Window::setVsync(bool enable) const
{
	SDL_GL_SetSwapInterval(enable ? 1 : 0);
}

void Window::dispatchToListeners(events::Event& event)
{
	for (events::IEventListener* eventListener : m_eventListeners)
	{
		eventListener->onEvent(event);
	}
}
