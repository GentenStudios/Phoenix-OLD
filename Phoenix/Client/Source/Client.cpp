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
#include <Client/Game.hpp>
#include <Client/SplashScreen.hpp>

using namespace phx::client;
using namespace phx;

Client::Client() : m_window("Phoenix Game!", 1280, 720), m_layerStack(&m_window)
{
	m_window.registerEventListener(this);
}

void Client::pushLayer(gfx::Layer* layer)
{
	if (layer->isOverlay())
	{
		m_layerStack.pushOverlay(layer);
	}
	else
	{
		m_layerStack.pushLayer(layer);
	}
}

void Client::popLayer(gfx::Layer* layer)
{
	if (layer->isOverlay())
	{
		m_layerStack.popOverlay(layer);
	}
	else
	{
		m_layerStack.popLayer(layer);
	}
}

void Client::onEvent(events::Event e)
{
	using namespace events;
	switch (e.type)
	{
	case EventType::KEY_PRESSED:
		switch (e.keyboard.key)
		{
		case Keys::KEY_Q:
			m_window.close();
			e.handled = true;
			break;
		case Keys::KEY_P:
			m_debugOverlayActive = !m_debugOverlayActive;
			if (m_debugOverlayActive)
			{
				if (m_debugOverlay == nullptr)
					m_debugOverlay = new DebugOverlay();

				m_layerStack.pushLayer(m_debugOverlay);
			}
			else
			{
				m_layerStack.popLayer(m_debugOverlay);
			}
			// don't set this to handled so we can propagate this down the stack
			// to enable debug overlays.
			// e.handled = true;
			break;
		default:
			break;
		}
		break;
	case EventType::LAYER_DESTROYED:
		if (std::string(e.layer) == "SplashScreen")
		{
			Game* game = new Game(&m_window);
			m_layerStack.pushLayer(game);
			e.handled = true;
		}
		break;
	default:
		break;
	}

	if (!e.handled)
	{
		m_layerStack.onEvent(e);
	}
}

void Client::run()
{
	Settings::get()->load("settings.txt");

	SplashScreen* splashScreen = new SplashScreen();
	m_layerStack.pushLayer(splashScreen);

	std::size_t last = SDL_GetPerformanceCounter();
	while (m_window.isRunning())
	{
		const std::size_t now = SDL_GetPerformanceCounter();
		const float       dt  = static_cast<float>(now - last) /
		                 static_cast<float>(SDL_GetPerformanceFrequency());
		last = now;

		m_window.startFrame();

		if (!m_layerStack.empty())
			m_layerStack.tick(dt);

		m_window.endFrame();
	}

	Settings::get()->save("settings.txt");
}
