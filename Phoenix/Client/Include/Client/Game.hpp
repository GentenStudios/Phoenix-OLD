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

#include <Client/GameTools.hpp>
#include <Client/Graphics/Camera.hpp>
#include <Client/Graphics/Layer.hpp>
#include <Client/Graphics/ShaderPipeline.hpp>
#include <Client/Graphics/UI.hpp>
#include <Client/Graphics/Window.hpp>
#include <Client/Player.hpp>

namespace phx::client
{
	/**
	 * @brief The actual game class for the Client.
	 *
	 * This is the class which actually implements the "game". The Client
	 * class is just a "runner" or an intermediary medium that runs all the
	 * ticking functions and manages all the layers, but this actually
	 * renders the voxel world and everything related to it.
	 *
	 * The other layers such as SplashScreen are not actually the game, but
	 * you know... just a SplashScreen - this is the main layer you actually
	 * interact with and play on.
	 *
	 * @see Layer
	 * @see LayerStack
	 */
	class Game : public gfx::Layer
	{
	public:
		explicit Game(gfx::Window* window);
		~Game() override;

		void onAttach() override;
		void onDetach() override;

		void onEvent(events::Event& e) override;
		void tick(float dt) override;

	private:
		gfx::Window*       m_window;
		gfx::FPSCamera*    m_camera = nullptr;
		Player*            m_player;
		voxels::ChunkView* m_world = nullptr;

		gfx::ShaderPipeline m_renderPipeline;

		ui::ChatWindow* m_chat = nullptr;

		GameTools* m_gameDebug = nullptr;
		bool       m_followCam = true;
		math::vec3 m_prevPos;
		int        m_playerHand = 0;
	};
} // namespace phx::client

