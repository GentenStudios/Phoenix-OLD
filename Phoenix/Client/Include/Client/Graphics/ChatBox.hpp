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

#include <Client/Graphics/Window.hpp>

#include <Common/Util/BlockingQueue.hpp>

#include <deque>
#include <functional>
#include <string>

namespace phx::gfx
{
	/**
	 * @brief The chat box displayed on screen.
	 *
	 * Terminology:
	 *		DrawBox = the main VISIBLE box for the chat, for example, the input
	 *			bar is in the DrawBox - if the box is not drawn there will be no
	 *			input box and no dark tint.
	 *
	 *		InvsibibleMessage = the messages shown when you don't have the chat
	 *			open. No background, just straight up text shown above whatever is
	 *			being rendered. Shown when the DrawBox is not being... drawn.
	 *
	 * This class holds chat messages (and history) for the client, and renders
	 * said history.
	 *
	 * @todo Implement way of pushing messages even when network doesn't exist.
	 *		Potentially have another BlockingQueue in that scenario and pass that
	 *		between something like the commander and ChatBox.
	 */
	class ChatBox
	{
	public:
		/**
		 * @brief Constructs a ChatBox.
		 * @param window The window being rendered in (used for sizing).
		 * @param messageQueue The message queue used when networking.
		 *
		 * @note messageQueue can be nullptr.
		 */
		ChatBox(Window* window, BlockingQueue<std::string>* messageQueue);
		~ChatBox();

		/**
		 * @brief Sets whether the main box should be drawn.
		 * @param drawBox Whether the box should be drawn or not.
		 */
		void setDrawBox(bool drawBox);
		
		/**
		 * @brief Gets whether the main box is being drawn or not.
		 * @return Whether the box is being drawn.
		 */
		bool shouldDrawBox();

		/**
		 * @brief Draws either the main box or the invisible buffer.
		 *
		 * @note Use tick instead of this directly, tick will make sure timings
		 * work and will actually remove invisibly buffered messages as time
		 * passes.
		 *
		 * If the box is drawn, the invisibility buffer will be cleared
		 * (unnecessary memory usage).
		 */
		void draw();

		/**
		 * @brief Ticks the Chatbox + draws it.
		 * @param dt The delta time from the previous frame.
		 *
		 * This calls draw, you do not need to call it again.
		 *
		 * This method will remove messages older than 5 seconds from the
		 * invisibility buffer, will drain the messageQueue (if applicable) and
		 * will update chat history and clip history length to the desired size
		 * (currently hard coded, should be configurable in the future.)
		 */
		void tick(float dt);

		/**
		 * @brief Sets a callback to be called on a new message.
		 * @param callback The callback to call when enter is pressed on the input.
		 *
		 * Empty messages will be automatically ignored.
		 *
		 * This will make sure a specific callback is called if you press enter
		 * on the input box. It will most likely be used to submit to network or
		 * command parsing objects.
		 */
		void setMessageCallback(
		    const std::function<void(const std::string& message)>& callback);

	private:
		/**
		 * @brief Stores the message and the time it's been active for.
		 */
		struct InvisibleMessage
		{
			float       time;
			std::string message;
		};

	private:
		Window* m_window = nullptr;

		bool m_drawBox = false;
		bool m_scroll  = false;

		char* m_input = nullptr;

		std::function<void(const std::string& message)> m_callback;

		BlockingQueue<std::string>*  m_messageQueue = nullptr;
		std::deque<std::string>      m_history;
		std::deque<InvisibleMessage> m_invisibleBuf;
	};
} // namespace phx::gfx
