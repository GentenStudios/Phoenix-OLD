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

#include <Client/Graphics/ChatBox.hpp>

#include <imgui.h>

#include <cstring>

using namespace phx::gfx;

// the maximum input size in the ImGui widget.
static constexpr std::size_t MAX_INPUT_LIMIT = 500;

ChatBox::ChatBox(Window* window, phx::BlockingQueue<std::string>* messageQueue)
    : m_window(window), m_messageQueue(messageQueue)
{
	m_input = new char[MAX_INPUT_LIMIT];
	std::memset(m_input, 0, MAX_INPUT_LIMIT);
}

ChatBox::~ChatBox() { delete[] m_input; }

void ChatBox::setDrawBox(bool drawBox) { m_drawBox = drawBox; }

bool ChatBox::shouldDrawBox() { return m_drawBox; }

void ChatBox::draw()
{
	// make it more readable.
	// clang-format off

	if (m_drawBox)
	{
		// we should clear it since opening the chat should remove anything from
		// there.
		m_invisibleBuf.clear();

		ImGui::PushStyleColor(ImGuiCol_WindowBg,       IM_COL32(20, 20, 20, 200));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive,  IM_COL32(20, 20, 70, 200));
		ImGui::PushStyleColor(ImGuiCol_FrameBg,        IM_COL32( 0,  0,  0, 200));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  IM_COL32( 0,  0,  0, 200));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32( 0,  0,  0, 200));

		bool open = true;

		ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2(m_window->getSize().x - 20, 250));
		ImGui::Begin("Chat", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
			ImGui::BeginChild("", ImVec2(ImGui::GetContentRegionAvail().x, 200), true);
				for (const auto& message : m_history)
				{
					ImGui::TextUnformatted(message.c_str());
				}

				if (m_scroll)
				{
					// scrolls to the bottom. 0.5f is the centre.
					ImGui::SetScrollHereY(1.f);
					m_scroll = false;
				}
			ImGui::EndChild();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::InputText("", m_input, MAX_INPUT_LIMIT, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				// imgui null terminates the string so std::string will load it fine.
				const std::string message = m_input;
				if (!message.empty())
				{
					m_history.emplace_back(message);

					if (m_callback)
					{
						m_callback(message);
					}

					// reset the array to 0, otherwise ImGui will NOT clear it and the message will stay in the box even after pressing enter.
					std::memset(m_input, 0, MAX_INPUT_LIMIT);

					// auto scroll if you type a message, otherwise leave it.
					m_scroll = true;
				}

				// tells it to refocus the keyboard after typing a command.
				ImGui::SetKeyboardFocusHere(-1);
			}
		ImGui::End();

		for (std::size_t i = 0; i < 5; ++i)
		{
			ImGui::PopStyleColor();
		}
	}
	else
	{
		// just render new text as it comes.
		// and drop the top of a buffer every 5 seconds or something.

		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));

		ImGui::SetNextWindowPos(ImVec2(20, 10));
		ImGui::SetNextWindowSize(ImVec2(0, 0));

		bool open = true;
		ImGui::Begin("ok", &open, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
			for (auto& element : m_invisibleBuf)
			{
				ImGui::TextUnformatted(element.message.c_str());
			}
		ImGui::End();

		ImGui::PopStyleColor();
	}

	// clang-format on
}

void ChatBox::tick(float dt)
{
	static constexpr std::size_t MAX_CHAT_HISTORY = 30;
	static constexpr std::size_t MAX_INVISIBLE_CHAT_HISTORY = 10;

	if (m_messageQueue)
	{
		std::string message;
		while (m_messageQueue->try_pop(message))
		{
			// don't emplace here since if it gets moved then we won't have
			// anything to put in the main history.
			m_invisibleBuf.push_back({0.f, message});

			if (m_drawBox)
			{
				// emplace here since it's the end of the chain, moving is fine
				// since an "empty" string is fine.
				m_history.emplace_back(message);
			}
		}
	}
	
	// limit history to 30, can be increased in the future.
	while (m_history.size() > MAX_CHAT_HISTORY)
	{
		m_history.pop_front();
	}

	// limit the invisible buffer to 10 (the text displayed when the chatbox is
	// not open).
	while (m_invisibleBuf.size() > MAX_INVISIBLE_CHAT_HISTORY)
	{
		m_invisibleBuf.pop_front();
	}

	for (auto& element : m_invisibleBuf)
	{
		element.time += dt;
	}

	while (!m_invisibleBuf.empty())
	{
		static constexpr float TIME_BEFORE_MESSAGE_REMOVED = 5.f;
		
		// keep messages for 5 seconds, otherwise discard them.
		if (m_invisibleBuf.front().time >= TIME_BEFORE_MESSAGE_REMOVED)
		{
			m_invisibleBuf.pop_front();
		}
		else
		{
			break;
		}
	}

	draw();
}

void ChatBox::setMessageCallback(
    const std::function<void(const std::string& message)>& callback)
{
	m_callback = callback;
}
