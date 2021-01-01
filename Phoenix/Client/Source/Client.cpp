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
#include <Client/Game/Timestep.hpp>

#include <Common/Logger.hpp>
#include <Common/Settings.hpp>

using namespace phx::client;

Client::Client() : m_window("Phoenix Game!", 1280, 720), m_layerStack(&m_window)
{
	m_window.registerEventListener(this);
}

void Client::setupCLIParam(phx::CLIParser* parser)
{
	phx::CLIParameter saveParam;
	saveParam.parameter       = "save";
	saveParam.shorthand       = "s";
	saveParam.enableShorthand = true;
	saveParam.helpString =
	    "Usage: \n\tPhoenixClient --save NameOfSaveToUse"
	    "\n\tAlternatively: PhoenixClient -s NameOfSaveToUse";

	phx::CLIParameter modParam;
	modParam.parameter  = "mods";
	modParam.multiValue = true;
	modParam.helpString = "Usage: \n\tPhoenixClient --mods Mod1 Mod2 Mod3 Mod4";

	phx::CLIParameter configParam;
	configParam.parameter       = "config";
	configParam.shorthand       = "c";
	configParam.enableShorthand = true;
	configParam.helpString =
	    "Usage: \n\tPhoenixClient --config PathToConfigFileToUse"
	    "\n\tAlternatively: PhoenixClient -c PathToConfigFileToUse";

	parser->addParameter(saveParam);
	parser->addParameter(modParam);
	parser->addParameter(configParam);

	m_cliArguments = parser;
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

	LoggerConfig config;
	config.logToFile = true;
	config.logFile   = "PhoenixClient.log";
	config.verbosity = LogVerbosity::DEBUG;
	Logger::initialize(config);

	Game* game = new Game(&m_window, &m_registry);
	m_layerStack.pushLayer(game);

	Timestep timer;
	while (m_window.isRunning())
	{
		m_window.startFrame();

		if (!m_layerStack.empty())
			m_layerStack.tick(timer.step());

		m_window.endFrame();
	}
}
