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

#include <deque>
#include <string>
#include <mutex>
#include <functional>

namespace phx::gfx
{
	class ChatBox
	{
	public:
		ChatBox(Window* window);
		~ChatBox();

		void setDrawBox(bool drawBox);
		bool shouldDrawBox();

		void draw();

		void tick(float dt);

		void setMessageCallback(
		    const std::function<void(const std::string& message)>& callback);

		void pushMessage(const std::string& message);

	private:
		struct InvisibleMessage
		{
			float time;
			std::string message;
		};
		
	private:
		Window* m_window = nullptr;
		
		bool m_drawBox = false;
		bool m_scroll = false;

		char* m_input = nullptr;

		std::mutex m_mutex;

		std::function<void(const std::string& message)> m_callback;
		
		std::deque<std::string> m_history;
		std::deque<InvisibleMessage> m_invisibleBuf;
	};
} // namespace phx::gfx
