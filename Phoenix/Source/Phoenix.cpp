// Copyright 2020 Genten Studios
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

#include <Phoenix/Commander.hpp>
#include <Phoenix/ContentLoader.hpp>
#include <Phoenix/Phoenix.hpp>
#include <Phoenix/Settings.hpp>
#include <Phoenix/SplashScreen.hpp>

using namespace phx;

Commander kirk = Commander();

static void rawEcho(const std::string& input, std::ostringstream& cout)
{
	kirk.callback(input, cout);
}

Phoenix::Phoenix()
    : m_chat(ui::ChatWindow("Chat Window", 5,
                            "Type something and hit enter to run a command!\n"))
{
	m_window = new gfx::Window("Phoenix Game!", 1280, 720);
	m_window->registerEventListener(this);

	m_chat.registerCallback(&rawEcho);

	/**
	 * @addtogroup luaapi
	 *
	 * @subsubsection coreprint core.print(text)
	 * @brief Prints text to the players terminal
	 *
	 * @param text The text to be outputted to the terminal
	 *
	 */
	ContentManager::get()->lua["core"]["print"] =
	    [this](const std::string& text) { m_chat.cout << text << "\n"; };
}

Phoenix::~Phoenix() { delete m_window; }

void Phoenix::onEvent(events::Event e)
{
	m_layerStack.onEvent(e);
}

void Phoenix::run()
{
	Settings::get()->load();

	game::SplashScreen* splashScreen = new game::SplashScreen();
	m_layerStack.pushLayer(splashScreen);

	float last = static_cast<float>(SDL_GetTicks());
	while (m_window->isRunning())
	{
		const float now = static_cast<float>(SDL_GetTicks());
		const float dt  = (now - last) / 1000.f;
		last            = now;

		m_window->startFrame();

		if (!m_layerStack.empty())
			m_layerStack.tick(dt);
		else
			m_window->close();

		m_window->endFrame();
	}

	Settings::get()->save();
}
