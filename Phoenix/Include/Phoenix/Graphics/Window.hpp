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

#pragma once

#include <Phoenix/Events/Event.hpp>
#include <Phoenix/Events/IEventListener.hpp>
#include <Phoenix/Math/Math.hpp>

#include <SDL.h>

#include <functional>
#include <vector>

namespace q2
{
	namespace gfx
	{
		enum class CursorState : int
		{
			NORMAL,  /// @brief Sets the cursor back to "normal", visible &
			         /// controllable, not locked to the window at all.
			HIDDEN,  /// @brief Hides the cursor, but allows "normal" use.
			DISABLED /// @brief Disables the cursor, so it isn't visible,
			         /// currently has the same use as HIDDEN, however they mean
			         /// different in different windowing libraries.
		};

		class Window
		{
		public:
			Window(const std::string& title, int width, int height);
			~Window();

			void pollEvents();
			void swapBuffers() const;

			bool isRunning() const;
			void startFrame();
			void endFrame();

			void show() const;
			void hide() const;
			void maximize() const;
			void minimize() const;
			void focus() const;
			void close();

			void        resize(math::vec2i size);
			math::vec2i getSize() const;
			void        setResizable(bool enabled);

			void setVSync(bool enabled);
			bool isVSync() const;

			void setTitle(const std::string& title) const;

			void setFullscreen(bool enabled);
			bool isFullscreen() const;

			void registerEventListener(events::IEventListener* listener);
			void nullifyEventListener(events::IEventListener* listener);
			bool isKeyDown(events::Keys key) const;

			void        setCursorState(gfx::CursorState state);
			void        setCursorPosition(math::vec2i pos);
			math::vec2i getCursorPosition() const;

		private:
			SDL_Window*   m_window;
			SDL_GLContext m_context;

			bool m_running;

			bool m_vsync;
			bool m_fullscreen;

			math::vec2i m_cachedScreenSize;

		private:
			void dispatchToListeners(events::Event& event);
			std::vector<events::IEventListener*> m_eventListeners;
		};
	} // namespace gfx
} // namespace q2