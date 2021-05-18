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

#include <Client/UI/EscapeMenu.hpp>
#include <Client/UI/GameTools.hpp>

#include <Client/Graphics/ChatBox.hpp>
#include <Client/Graphics/ChunkRenderer.hpp>
#include <Client/Graphics/Camera.hpp>
#include <Client/Graphics/Layer.hpp>
#include <Client/Graphics/ShaderPipeline.hpp>
#include <Client/Graphics/SkyboxRenderer.hpp>
#include <Client/Graphics/Window.hpp>
#include <Client/InputQueue.hpp>
#include <Client/UI/HUD.hpp>
#include <Client/Voxels/BlockRegistry.hpp>

#include "Client/UI/InventoryUI.hpp"
#include <Client/Voxels/ItemRegistry.hpp>
#include <Common/CMS/ModManager.hpp>
#include <Common/Save.hpp>
#include <Common/Voxels/InventoryManager.hpp>

#include <Client/Graphics/SelectionboxRenderer.hpp>
#include <Client/Voxels/AudioEventHandler.hpp>
#include <soloud.h>
#include <soloud_wav.h>

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
		explicit Game(gfx::Window* window, entt::registry* registry,
		              bool networked = false);
		~Game() override;

		void onAttach() override;
		void onDetach() override;

		void onEvent(events::Event& e) override;
		void tick(float dt) override;

	private:
		/**
		 * @brief This confirms that the prediction on the client was accurate
		 * to what the server decided to accept and send back in a confirmation.
		 *
		 * @param position The current player position to confirm
		 *
		 * @note This is a rough implementation, ideally it doesn't live in the
		 * main game class forever but it is easier to work on it with access to
		 * the values it needs.
		 */
		void confirmState(const Position& position);

	private:
		AudioRegistry      m_audioRegistry;
		AudioEventHandler* m_audioEventHandler;

		BlockRegistry m_blockRegistry;
		ItemRegistry  m_itemRegistry;

		entt::registry*    m_registry;
		entt::entity       m_player;
		voxels::Inventory* m_playerInventory = nullptr;
		int                m_playerHand      = 0;

		SoLoud::Soloud m_soloud;

		gfx::Window*        m_window;
		gfx::FPSCamera*     m_camera        = nullptr;

		gfx::ChunkRenderer        m_chunkRenderer;
		gfx::SkyboxRenderer       m_skyboxRenderer;
		gfx::SelectionboxRenderer m_selectionboxRenderer;

		gfx::ChatBox* m_chat = nullptr;

		cms::ModManager* m_modManager;

		HUD*         m_hud        = nullptr;
		InventoryUI* m_inventory  = nullptr;
		EscapeMenu*  m_escapeMenu = nullptr;
		GameTools*   m_gameDebug  = nullptr;
		bool         m_followCam  = true;
		math::vec3   m_prevPos;

		client::Network*    m_network    = nullptr;
		client::InputQueue* m_inputQueue = nullptr;

		// This is an internal log of the recent input states sent to the server
		std::list<InputState> m_states;

        Save* m_save = nullptr;
        voxels::Map*              m_map        = nullptr;
        voxels::InventoryManager* m_invManager = nullptr;
	};
} // namespace phx::client
