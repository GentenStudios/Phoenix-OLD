// Copyright 2019 Genten Studios
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

#include <Phoenix/Graphics/ChunkRenderer.hpp>
#include <Phoenix/Voxels/Chunk.hpp>

#include <vector>

namespace phx
{
	namespace voxels
	{
		/**
		 * @brief Generation and Rendering manager for the "world".
		 *
		 * The ChunkManager handles generation of new chunks as you wander
		 * around as well as the rendering of these chunks using a ChunkRenderer
		 * object.
		 *
		 * The ChunkManager requires a position to be sent in each tick event,
		 * however it should be kept in mind that the "Camera" position and the
		 * voxel-world space coordinates are INCOMPATIBLE. This is due to the
		 * vertices required by the rendering system. The tick function
		 * currently automates the process of converting the camera position
		 * into the world space coordinates, however this should be phased out
		 * in the future and made much more explicit, just to be on the safer
		 * side.
		 *
		 * @paragraph Usage
		 * @code
		 * ChunkManager world(viewDistance);
		 *
		 * mainGameLoop()
		 * {
		 *     camera.tick(dt);
		 *
		 *     world.tick(camera.getPosition());
		 *
		 *     world.render();
		 * }
		 * @endcode
		 */
		class ChunkManager
		{
		public:
			/**
			 * @brief Constructs the ChunkManager
			 * @param viewDistance The view distance in every direction.
			 */
			ChunkManager(int viewDistance);
			~ChunkManager();

			/**
			 * @brief Updates visible chunk depending on player position.
			 * @param playerPos The position of the player, should be straight from the camera - conversion calculations done internally,.
			 *
			 * This function will load/unload chunks as required while moving
			 * around as the player. When the player position is sent, it should
			 * be raw from the camera, since the conversion calculations from
			 * camera coordinates to voxel coordinates is done internally. This
			 * needs to be repaired and done explicitly through external code.
			 *
			 * @todo Create classes to solve said issue, decide on whether to rely on explicit or implicit conversion of coordinate systems.
			 */
			void tick(math::vec3 playerPos);

			/**
			 * @brief Renders active chunks.
			 */
			void render();

		private:
			int m_viewDistance = 1; // 1 chunk

			std::vector<Chunk>  m_activeChunks;
			gfx::ChunkRenderer* m_renderer;
		};
	} // namespace voxels
} // namespace phx
