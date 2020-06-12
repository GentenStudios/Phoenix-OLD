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
	const math::vec2 size = window->getSize();
	m_windowCentre         = {size.x / 2, size.y / 2};
}

EscapeMenu::~EscapeMenu()
{
	// delete them just in case onDetach is never called.
	delete m_button;
	delete m_container;
}

void EscapeMenu::onEvent(events::Event& e)
{
	if (e.type == events::EventType::WINDOW_RESIZED)
	{
		m_windowCentre = {e.size.width / 2, e.size.height / 2};
		// dont set handled because we want this to propagate down, this event
		// isn't being handled, we're just using the data for our own help.
	}

	m_container->onEvent(e);
}

void EscapeMenu::onAttach()
{
	m_container =
	    new gui::Container("", {50, 50}, {30, 30}, {0, 0, 0}, 1.f, m_window);

	m_button = new gui::Button(m_container, {50, 50}, {90, 100},
	                           {128, 128, 128}, 1.f);

	m_button->setCallback([this](const events::Event& e)
	{
		if (e.mouse.button == events::MouseButtons::LEFT)
		{
			m_window->close();
		}
	});
}

void EscapeMenu::onDetach()
{
	delete m_button;
	m_button = nullptr;

	delete m_container;
	m_container = nullptr;
}

void EscapeMenu::tick(float dt) { m_container->tick(dt); }
