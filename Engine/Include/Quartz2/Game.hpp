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

#pragma once

#include <cstddef>
#include <string>

#include <SDL.h>

namespace q2
{
	// We do this here to avoid having to include SDL_opengl.h in the header.
	// This is because SDL_opengl.h defines __gl_h_ which glad doesn't like
	// and so complains & fails the build ("OpenGL header already incldued...." etc...)
	// So to avoid having to include glad.h/OpenGL32.hpp in this header above SDL_opengl.h
	// since the only thing we need is the SDL_GLContext symbol, just declare it as so here.
	typedef void* SDL_GLContext;

	class Game
	{
	public:
		Game (std::size_t windowWidth, std::size_t windowHeight, const std::string& windowTitle);

		void start();

		std::size_t getWindowWidth() const
			{ return m_windowWidth; }

		std::size_t getWindowHeight() const
			{ return m_windowHeight; }

		SDL_Window* getSDLWindow() const
			{ return m_sdlWindow; }

		int getFPS() const
			{ return m_fps; }

	protected:
		virtual void onStart() {}
		virtual void onExit() {}
		virtual void onFrame(float dt) {}
		virtual bool onEvent(SDL_Event e) { return false; }

		void exitGame();

	private:
		SDL_Window* m_sdlWindow;
		SDL_GLContext m_sdlGLContext;

		std::size_t m_windowWidth;
		std::size_t m_windowHeight;
		std::string m_windowTitle;

		int m_fps;
	};
}