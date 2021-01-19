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

EscapeMenu::EscapeMenu(phx::gfx::Window* window)
    : phx::gfx::Overlay("EscapeMenu"), m_window(window)
{
	// all settings have definitely been loaded by now.
	// get the final settings list from settings instance.
	m_settings = Settings::instance()->getImplFinalSettings();
}

void EscapeMenu::onAttach()
{
	m_page = Page::MAIN;
}

void EscapeMenu::onDetach() {}

void EscapeMenu::onEvent(phx::events::Event& e)
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
	static const double             DoubleMax = 100.0;
	static const double             DoubleMin = 0.0;
	static const unsigned long long IntMax    = 100;
	static const unsigned long long IntMin    = 0;
	
	ImGui::SetNextWindowPos({m_window->getSize().x / 2 - WIDTH / 2,
	                         m_window->getSize().y / 2 - HEIGHT / 2});

	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize);

	switch (m_page)
	{
	case Page::MAIN:
		if (ImGui::Button("Settings", {290, 30}))
		{
			m_page = Page::SETTINGS;
		}
		if (ImGui::Button("Exit", {290, 30}))
		{
			m_window->close();
		}
		break;
	case Page::SETTINGS:		
		for (auto& setting : m_settings)
		{
			if (setting.val->is_boolean())
			{
				ImGui::Checkbox(setting.key.c_str(),
				                setting.val->get_ptr<bool*>());
			}
			else if (setting.val->is_number_float())
			{
				double* val = setting.val->get_ptr<double*>();
				ImGui::SliderScalar(
				    setting.key.c_str(), ImGuiDataType_Double,
				    static_cast<void*>(setting.val->get_ptr<double*>()),
				    static_cast<const void*>(&DoubleMin),
				    static_cast<const void*>(&DoubleMax), "%.2f");
			}
			else if (setting.val->is_number_integer())
			{
				// use long long int to allow negative.
				ImGui::SliderScalar(
				    setting.key.c_str(), ImGuiDataType_U64,
				    static_cast<void*>(setting.val->get_ptr<std::size_t*>()),
				    static_cast<const void*>(&IntMin),
				    static_cast<const void*>(&IntMax));
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
