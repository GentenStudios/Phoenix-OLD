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

#include <Client/Client.hpp>
#include <Client/Graphics/Window.hpp>
#include <Common/Util/BlockingQueue.hpp>

#include <imgui.h>

using namespace phx;

// clang-format off

struct InvisibleMessage
{
	float time;
	std::string message;
};

class ChatBox : public events::IEventListener
{
public:
	explicit ChatBox(gfx::Window* window) : m_window(window) {}
	~ChatBox() = default;

	void onEvent(events::Event e) override
	{
		if (e.type == events::EventType::KEY_PRESSED)
		{
			if (e.keyboard.key == events::Keys::KEY_F2)
			{
				m_drawBox = !m_drawBox;
			}
		}
	}

	void draw()
	{
		if (m_drawBox)
		{
			// we should clear it since opening the chat should remove anything from there.
			m_invisibilityBuffer.clear();
			
			ImGui::PushStyleColor(ImGuiCol_WindowBg,       IM_COL32(20, 20, 20, 200));
			ImGui::PushStyleColor(ImGuiCol_TitleBgActive,  IM_COL32(20, 20, 70, 200));
			ImGui::PushStyleColor(ImGuiCol_FrameBg,        IM_COL32(0, 0, 0, 200));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  IM_COL32(0, 0, 0, 200));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0, 0, 0, 200));

			bool open = true;

			ImGui::SetNextWindowPos(ImVec2(10, 10));
			ImGui::SetNextWindowSize(ImVec2(m_window->getSize().x - 20, 250));
			ImGui::Begin("Chat", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
				ImGui::BeginChild("", ImVec2(ImGui::GetContentRegionAvail().x, 200), true);
					for (const auto& message : m_history)
					{
						ImGui::TextUnformatted(message.c_str());
					}

					if (m_shouldScroll)
					{
						ImGui::SetScrollHereY(1.f);
						m_shouldScroll = false;
					}
				ImGui::EndChild();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::InputText("", m_inputArray, MAX_INPUT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				const std::string message = m_inputArray;
				if (!message.empty())
				{
					m_history.push_back(m_inputArray);
					memset(m_inputArray, 0, sizeof(m_inputArray));

					m_shouldScroll = true;
				}

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
				for (auto& element : m_invisibilityBuffer)
				{
					ImGui::TextUnformatted(element.message.c_str());
				}
			ImGui::End();

			ImGui::PopStyleColor();
		}
	}

	void tick(float dt)
	{
		// limit history to 30, eventually we can just use a ring buffer.
		while (m_history.size() > 30)
		{
			m_history.pop_front();
		}

		for (auto& element : m_invisibilityBuffer)
		{
			element.time += dt;
		}

		while (!m_invisibilityBuffer.empty())
		{			
			if (m_invisibilityBuffer.front().time >= 5.f)
			{
				m_invisibilityBuffer.pop_front();
			}
			else
			{
				break;
			}
		}

		draw();
	}
	
	void pushMessage(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_invisibilityBuffer.push_back({0, message});
		m_history.push_back(message);
	}
	
	std::deque<InvisibleMessage> m_invisibilityBuffer;
private:
	gfx::Window* m_window;
	
	bool m_drawBox = false;
	bool m_shouldScroll = false;

	static constexpr std::size_t MAX_INPUT_SIZE = 500;
	char m_inputArray[MAX_INPUT_SIZE] = {};

	std::mutex m_mutex;
	
	std::deque<std::string> m_history;
};

bool running = true;

void otherThreadPush(ChatBox* chat)
{
	while (running)
	{
		chat->pushMessage("wowee test poopoopoopoo");
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	}
}

#undef main
int main(int argc, char** argv)
{
	// client::Client::get()->run();

	gfx::Window window("poopoo", 1280, 720);

	ChatBox chat(&window);
	window.registerEventListener(&chat);

	std::thread t1(otherThreadPush, &chat);

	// clang-format off
	std::size_t last = SDL_GetPerformanceCounter();
	while (window.isRunning())
	{
		const std::size_t now = SDL_GetPerformanceCounter();
		const float       dt  = static_cast<float>(now - last) /
		                 static_cast<float>(SDL_GetPerformanceFrequency());
		last = now;

		window.startFrame();

		chat.tick(dt);
		
		window.endFrame();
	}

	running = false;
	t1.join();
	
	return 0;
}
