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

#include <Client/Events/IEventListener.hpp>
#include <Client/Graphics/LayerStack.hpp>
#include <Client/Graphics/Window.hpp>
#include <Client/UI/DebugOverlay.hpp>

#include <Common/CLIParser.hpp>
#include <Common/Settings.hpp>
#include <Common/Singleton.hpp>

#include <entt/entt.hpp>

namespace phx::client
{
	class Client : public events::IEventListener, public Singleton<Client>
	{
	public:
		Client();
		~Client() = default;

		void setupCLIParam(CLIParser* parser);

		void pushLayer(gfx::Layer* layer);
		void popLayer(gfx::Layer* layer);
		bool isDebugLayerActive() const { return m_debugOverlayActive; }

		void onEvent(events::Event e) override;
		void run();

	private:
		CLIParser* m_cliArguments = nullptr;

		entt::registry m_registry;

		gfx::LayerStack m_layerStack;
		gfx::Window     m_window;

		bool          m_debugOverlayActive = false;
		DebugOverlay* m_debugOverlay       = nullptr;
	};
} // namespace phx::client
