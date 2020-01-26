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
#include <Phoenix/Math/Math.hpp>
#include <Phoenix/Voxels/Chunk.hpp>

#include <vector>

namespace phx
{
	namespace voxels
	{
		class ChunkManager
		{
		public:
			ChunkManager(BlockType* defaultBlockType, unsigned int seed);
			ChunkManager(ChunkManager&& other) = default;

			~ChunkManager() = default;

			void tick(math::vec3 position);
			void testGeneration();

			void       setBlockAt(math::vec3 position, BlockType* block);
			BlockType* getBlockAt(math::vec3 position) const;

			void breakBlockAt(math::vec3 position, BlockType* block);
			void placeBlockAt(math::vec3 position, BlockType* block);

			void render();

		private:
			unsigned int m_seed;
			BlockType*   m_defaultBlockType;

			math::vec3 m_lastPos;
			
			std::vector<Chunk>  m_chunks;
			gfx::ChunkRenderer* m_renderer;
		};
	} // namespace voxels
} // namespace phx
