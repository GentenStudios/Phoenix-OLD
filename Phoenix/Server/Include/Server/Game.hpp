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

#include <Server/Commander.hpp>
#include <Server/Iris.hpp>
#include <Server/Voxels/BlockRegistry.hpp>

#include <Common/Voxels/Map.hpp>

#include <entt/entt.hpp>

namespace phx::server
{
	class Game
	{
	public:
		/** @brief The server side game object, this handles all of the core
		 * game logic.
		 *
		 * @param registry The shared EnTT registry
		 * @param running Pointer to a boolean, the threaded function only runs
		 * if this is true
		 * @param iris Pointer to the networking system
		 */
		Game(BlockRegistry* blockReg, entt::registry* registry, net::Iris* iris,
		     Save* save);

		~Game();

		/** @brief Loads all API's that the game utilizes into a CMS ModManager
		 *
		 * @param manager The mod manager to load the API into
		 */
		void registerAPI(cms::ModManager* manager);

		/**
		 * @brief Runs the main game loop as long as running is true
		 */
		void run();

		/**
		 * @brief Kills the main game loop.
		 */
		void kill();

		/// @brief Just a temporary static storage for the DT
		/// @TODO Move this to a config file
		static constexpr float dt = 1.f / 20.f;

	private:
		/// @brief The main loop runs while this is true
		bool m_running = false;
		/// @brief The block registry to use.
		BlockRegistry* m_blockRegistry;
		/// @breif An EnTT registry to store various data in
		entt::registry* m_registry;
		/// @brief The networking object to get data from
		net::Iris* m_iris;
		/// @brief A commander object to process commands
		Commander* m_commander;
		/// @brief The map the players exist on
		voxels::Map m_map;
	};
} // namespace phx::server