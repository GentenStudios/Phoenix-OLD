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

/**
 * @file Window.hpp
 * @brief The window object, also provides a surface to render to.
 *
 * @copyright Copyright (c) 2019-20 Genten Studios
 */

#pragma once

#include <Client/Events/Event.hpp>
#include <Client/Events/IEventListener.hpp>
#include <Common/Math/Math.hpp>

#include <SDL.h>

#include <functional>
#include <vector>

namespace phx
{
	namespace gfx
	{
		/**
		 * @brief Provides the functionality to disable/enable the cursor
		 */
		enum class CursorState : int
		{
			NORMAL,  /// @brief Sets the cursor back to "normal", visible &
			         /// controllable, not locked to the window at all.
			HIDDEN,  /// @brief Hides the cursor, but allows "normal" use.
			DISABLED /// @brief Disables the cursor, so it isn't visible,
			         /// currently has the same use as HIDDEN, however they mean
			         /// different in different windowing libraries.
		};

		/**
		 * @brief The Window to be rendering too.
		 *
		 * The window is cross-platform, however will not work on either
		 * Android or iOS since these are mobile platforms and we do not support
		 * them as of yet.
		 *
		 * These windows home an OpenGL context, which requires a minimum of
		 * OpenGL 3.3 to be present on a computer. Without this, the program
		 * will auto-exit and a user will have to find another computer to use
		 * that supports OpenGL 3.3.
		 *
		 * Note: Event handling **NEEDS** to be blocking otherwise LayerStack
		 * will have issues with the Layer destroyed event. - @beeperdeeper089
		 * (19/02/2020)
		 *
		 * @paragraph Usage
		 * @code
		 * Window* window = new Window("Phoenix!", 1280, 720);
		 * window->show();
		 *
		 * // check event documentation for more information on this.
		 * window->registerEventListener(eventListener);
		 *
		 * while(window->isRunning())
		 * {
		 *    window->startFrame();
		 *
		 *    // world.render or whatever here.
		 *
		 *    window->endFrame();
		 * }
		 *
		 * delete window;
		 */
		class Window
		{
		public:
			/**
			 * @brief Produces a window using provided parameters.
			 * @param title The window title.
			 * @param width The width of the window.
			 * @param height The height of the window.
			 *
			 * @todo Change from using 2 integers to a math::vec2i at some
			 * point.
			 */
			Window(const std::string& title, int width, int height);
			~Window();

			/**
			 * @brief Polls the system for any key, window or mouse events that
			 * may have occured since the last frame.
			 *
			 * This function runs the tied EventListeners' ``onEvent()``
			 * function for each event that has occured since the last frame.
			 * Any key presses, etc... will be sent. All the different types of
			 * events are documented in the events system.
			 */
			void pollEvents();

			/**
			 * @brief Swaps the OpenGL buffers to update what's on screen.
			 */
			void swapBuffers() const;

			/**
			 * @brief Tells whether the window has been told to close, either by
			 * an event or just calling ``close()``.
			 * @return Whether the window is still "running".
			 */
			bool isRunning() const;

			/**
			 * @brief Starts a new frame.
			 *
			 * This function is just a wrapper function that initializes a new
			 * ImGUI frame and clears the back buffer for more OpenGL rendering
			 * to occur. Eventually as more things are required, this will have
			 * more functionality.
			 */
			void startFrame();

			/**
			 * @brief Ends the current frame.
			 *
			 * This function is a wrapper function that swaps buffers and polls
			 * events for that frame. Seek information on those through the rest
			 * of the documentation.
			 */
			void endFrame();

			/**
			 * @brief Shows the window.
			 */
			void show() const;

			/**
			 * @brief Hides the window.
			 */
			void hide() const;

			/**
			 * @brief Maximizes the window.
			 */
			void maximize() const;

			/**
			 * @brief Minimizes the window.
			 */
			void minimize() const;

			/**
			 * @brief Requests window focus.
			 *
			 * This function requests window focus from the WM (Window Manager).
			 * This does not always guarantee focus since it is to the
			 * discretion of the WM to decide whether to fulfill the request.
			 */
			void focus() const;

			/**
			 * @brief Closes the window.
			 *
			 * This function does not immediately close the window, but the
			 * current frame being rendered will be rendered and then
			 * ``isRunning()`` will return false, meaning the main game loop
			 * will exit and resources can be de-initialized.
			 */
			void close();

			/**
			 * @brief Resizes the window.
			 * @param size The size to set the window.
			 */
			void resize(math::vec2i size);

			/**
			 * @brief Gets the size of the window.
			 * @return The size of the window.
			 */
			math::vec2i getSize() const;

			/**
			 * @brief Enables or Disables the ability to resize the window.
			 * @param enabled Whether to enable resizing or not.
			 */
			void setResizable(bool enabled);

			/**
			 * @brief Enables or Disables VSync.
			 * @param enabled Whether VSync should be enabled or not.
			 */
			void setVSync(bool enabled);

			/**
			 * @brief Gets whether VSync is enabled or not.
			 * @return Whether VSync is enabled or not.
			 */
			bool isVSync() const;

			/**
			 * @brief Sets a new title for the window.
			 * @param title The new title to set the window to.
			 */
			void setTitle(const std::string& title) const;

			/**
			 * @brief Sets whether fullscreen should be enabled or not.
			 * @param enabled Whether fullscreen should be enabled or not.
			 *
			 * This is NOT the same as maximized. Fullscreen means there is no
			 * bar with the close button, etc... at the top, it is a border to
			 * border window to render on.
			 */
			void setFullscreen(bool enabled);

			/**
			 * @brief Gets if the window is in fullscreen.
			 * @return Whether the window is in fullscreen or not.
			 */
			bool isFullscreen() const;

			/**
			 * @brief Registers a new event listener.
			 * @param listener The object to send events to.
			 *
			 * Event listeners should inherit from events::IEventListener, for
			 * example:
			 * @code
			 * class Phoenix : public events::IEventListener;
			 * @endcode
			 *
			 * They will all have an onEvent method which will execute at
			 * necessary.
			 */
			void registerEventListener(events::IEventListener* listener);

			/**
			 * @brief Removes an event listener from the window.
			 * @param listener The object to stop sending event events to.
			 */
			void nullifyEventListener(events::IEventListener* listener);

			/**
			 * @brief Dispatches a custom provided event to all listeners.
			 * @param e The event to send to listeners.
			 */
			void dispatchCustomEvent(events::Event e);

			/**
			 * @brief Queries whether a specific key is pressed.
			 * @param key The key to query.
			 * @return Whether the key is pressed or not.
			 */
			bool isKeyDown(events::Keys key) const;

			/**
			 * @brief Sets the cursor state.
			 * @param state The state to set the cursor to.
			 *
			 * Information for cursor states can be found in the documentation
			 * for gfx::CursorState.
			 */
			void setCursorState(gfx::CursorState state);

			/**
			 * @brief Sets the cursor position.
			 * @param pos The position in the window to set the cursor to.
			 */
			void setCursorPosition(math::vec2i pos);

			/**
			 * @brief Gets the position of the cursor.
			 * @return The position of the cursor.
			 */
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
} // namespace phx