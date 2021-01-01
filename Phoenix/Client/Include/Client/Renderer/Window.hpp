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

#include <Common/Math/Math.hpp>

#include <Client/Events/IEventListener.hpp>
#include <Client/Renderer/InputState.hpp>

#include <string>

namespace phx::render
{
	class Window
	{
	public:
		Window(const std::string& title, const math::vec2& size,
		       bool fullscreen);
		~Window();

		const InputState* getInputState() const;

		bool isRunning() const;
		void close();
		void swapBuffers();
		void pollEvents();
		void registerEventListener(events::IEventListener* listener);
		void removeEventListener(events::IEventListener* listener);

		void show();
		void hide();
		void maximise();
		void minimise();
		void focus();

		std::string getTitle() const;
		void        setTitle(const std::string& title);

		bool isFullscreen() const;
		void setFullscreen(bool enable);

		math::vec2 getSize() const;
		void       resize(const math::vec2& size);
		void       setResizable(bool enable);

		bool isVsync() const;
		void setVsync(bool enable) const;

	private:
		void dispatchToListeners(events::Event& event);

	private:
		bool m_running;

		SDL_Window*   m_window;
		SDL_GLContext m_context;

		InputState                           m_inputState = {};
		std::vector<events::IEventListener*> m_eventListeners;
	};
} // namespace phx::render
