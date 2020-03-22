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
#include <Client/EscapeMenu.hpp>

#include <imgui.h>

using namespace phx::client;
using namespace phx;

EscapeMenu::EscapeMenu(gfx::Window* window)
    : gfx::Overlay("EscapeMenu"), m_window(window)
{
	const math::vec2i size = window->getSize();
	m_windowCentre         = {size.x / 2, size.y / 2};
}

void EscapeMenu::onEvent(events::Event& e)
{
	if (e.type == events::EventType::WINDOW_RESIZED)
	{
		m_windowCentre = {e.size.width / 2, e.size.height / 2};
		// dont set handled because we want this to propagate down, this event
		// isn't being handled, we're just using the data for our own help.
	}
}

void EscapeMenu::onAttach() {}

void EscapeMenu::onDetach() {}

void EscapeMenu::tick(float dt)
{
	static bool p_open = true;

	ImGui::SetNextWindowPos({static_cast<float>(m_windowCentre.x),
	                         static_cast<float>(m_windowCentre.y)},
	                        0, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowBgAlpha(0.6f);

	if (ImGui::Begin(
	        "Escape Menu", &p_open,
	        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
	            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
	            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav))
	{
		if (ImGui::Button("Exit"))
		{
			m_window->close();
		}
	}

	ImGui::End();
}
