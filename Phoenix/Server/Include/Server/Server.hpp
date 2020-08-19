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

#include <Server/Game.hpp>
#include <Server/Iris.hpp>
#include <Server/User.hpp>
#include <Server/Voxels/BlockRegistry.hpp>

#include <Common/CMS/ModManager.hpp>

#include <entt/entt.hpp>
#include <enet/enet.h>

#include <array>
#include <string>

namespace phx::server
{
	class Server
	{
	public:
		/**
		 * @brief Core object for the server
		 *
		 * @param save The save we are loading
		 */
		Server(const std::string& save);
		~Server();

		/// @brief Main loop for the server
		void run();

	private:
		/// @brief central boolean to control if the game is running or not
		bool m_running = true;

		/// @brief The Server's block registry.
		BlockRegistry m_blockRegistry;
		
		/// @brief An EnTT registry to store various data in
		entt::registry m_registry;

		/// @brief The networking object, this listens for incoming data
		net::Iris* m_iris;

		/**
		 * @brief The server side game object, this handles all of the core game
		 * logic.
		 */
		Game* m_game;

		/// @brief The mod manager providing LUA API functionality
		cms::ModManager* m_modManager;

		Save* m_save;
	};
} // namespace phx::server
