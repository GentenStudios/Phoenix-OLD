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

#include <Phoenix/Graphics/ChunkMesher.hpp>
#include <Phoenix/Voxels/ChunkManager.hpp>

#include <iostream>

using namespace phx::voxels;
using namespace phx;

ChunkManager::ChunkManager(int viewDistance) : m_viewDistance(viewDistance)
{
	const int viewLength       = (viewDistance * 2) + 1;
	const int maxVisibleChunks = viewLength * viewLength * viewLength;

	m_renderer = new gfx::ChunkRenderer(maxVisibleChunks);
	m_renderer->buildTextureArray();
}

ChunkManager::~ChunkManager() { delete m_renderer; }

void ChunkManager::tick(math::vec3 playerPos)
{
	playerPos = playerPos / 2.f;
	playerPos += 0.5f;

	const int posX = static_cast<int>(playerPos.x) / Chunk::CHUNK_WIDTH;
	const int posY = static_cast<int>(playerPos.y) / Chunk::CHUNK_HEIGHT;
	const int posZ = static_cast<int>(playerPos.z) / Chunk::CHUNK_DEPTH;

	// Get diameter to generate for.
	const int chunkViewDistance = m_viewDistance;

	for (int x = -chunkViewDistance; x <= chunkViewDistance; x++)
	{
		for (int y = -chunkViewDistance; y <= chunkViewDistance; y++)
		{
			for (int z = -chunkViewDistance; z <= chunkViewDistance; z++)
			{
				math::vec3 chunkToCheck = {
				    static_cast<float>(x + posX),
				    static_cast<float>(y + posY),
				    static_cast<float>(z + posZ)};

				chunkToCheck =
				    chunkToCheck * static_cast<float>(Chunk::CHUNK_WIDTH);

				auto result =
				    std::find_if(m_activeChunks.begin(), m_activeChunks.end(),
				                 [chunkToCheck](const Chunk& o) -> bool {
					                 return o.getChunkPos() == chunkToCheck;
				                 });

				if (result == m_activeChunks.end())
				{
					m_activeChunks.emplace_back(chunkToCheck);
					m_activeChunks.back().autoTestFill();

					gfx::ChunkMesher mesher(chunkToCheck,
					                        m_activeChunks.back().getBlocks(),
					                        m_renderer->getTextureTable());

					mesher.mesh();

					m_renderer->submitChunk(mesher.getMesh(), chunkToCheck);
				}
			}
		}
	}
}

void ChunkManager::render() { m_renderer->render(); }
