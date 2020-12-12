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

EscapeMenu::EscapeMenu(gfx::Window* window, gfx::FPSCamera* camera)
    : gfx::Overlay("EscapeMenu"), m_window(window), m_camera(camera)
{
}

void EscapeMenu::onAttach()
{
	m_page = Page::MAIN;

	m_sensitivity        = Settings::get()->getSetting("camera:sensitivity");
	m_currentSensitivity = m_sensitivity->value();
	m_camera->enable(false);
	m_active = true;
}
void EscapeMenu::onDetach()
{
	m_active = false;
	m_camera->enable(true);
}
void EscapeMenu::onEvent(events::Event& e)
{
	switch (e.type)
	{
	case events::EventType::KEY_PRESSED:
		switch (e.keyboard.key)
		{
		case events::Keys::KEY_ESCAPE:
			switch (m_page)
			{
			case Page::MAIN:
				Client::get()->popLayer(this);
				e.handled = true;
				break;
			case Page::SETTINGS:
				m_page    = Page::MAIN;
				e.handled = true;
				break;
			}
		default:
			break;
		}
	default:
		break;
	}
}

void EscapeMenu::tick(float dt)
{
	ImGui::SetNextWindowPos(
	    {m_window->getSize().x / 2 - 150, m_window->getSize().y / 2 - 150},
	    ImGuiCond_Once);

	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize);
	switch (m_page)
	{
	case Page::MAIN:
		if (ImGui::Button("Settings", {290, 30}))
		{
			m_page = Page::SETTINGS;
		}
		if (ImGui::Button("Return", {290, 30}))
		{
			Client::get()->popLayer(this);
		}
		if (ImGui::Button("Exit", {290, 30}))
		{
			m_window->close();
		}
		break;
	case Page::SETTINGS:
		const std::unordered_map<std::string, Setting>& settings =
		    Settings::get()->getSettings();

		for (const auto& setting : settings)
		{
			int i = setting.second.value();
			ImGui::SliderInt(setting.second.getName().c_str(), &i,
			                 setting.second.getMin(), setting.second.getMax());
			if (i != setting.second.value())
			{
				Settings::get()->getSetting(setting.second.getKey())->set(i);
			}
		}
		if (ImGui::Button("Back", {290, 30}))
		{
			m_page = Page::MAIN;
		}
		break;
	}
	ImGui::End();
}
