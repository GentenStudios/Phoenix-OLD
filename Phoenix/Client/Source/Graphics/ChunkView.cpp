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

#include <Client/Graphics/ChunkView.hpp>
#include <Client/Graphics/ChunkMesher.hpp>

#include <Common/Voxels/BlockRegistry.hpp>

#include <utility>

using namespace phx::voxels;
using namespace phx;

ChunkView::ChunkView(int viewDistance, Map&& map)
    : m_viewDistance(viewDistance), m_map(std::move(map))
{
	// calculates the maximum visible chunks.
	const int viewLength       = (viewDistance * 2) + 1;
	const int maxVisibleChunks = viewLength * viewLength * viewLength;

	m_renderer = new gfx::ChunkRenderer(maxVisibleChunks);
	m_renderer->buildTextureArray();
}

ChunkView::~ChunkView() { delete m_renderer; }

void ChunkView::tick(math::vec3 playerPos)
{
	// this block converts the raw camera/player position into voxel-world
	// positions.
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
				math::vec3 chunkToCheck = {static_cast<float>(x + posX),
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
					m_activeChunks.emplace_back(m_map.getChunk(chunkToCheck));

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

void ChunkView::render() { m_renderer->render(); }

BlockType* ChunkView::getBlockAt(math::vec3 position) const
{
	int posX = static_cast<int>(position.x / Chunk::CHUNK_WIDTH);
	int posY = static_cast<int>(position.y / Chunk::CHUNK_HEIGHT);
	int posZ = static_cast<int>(position.z / Chunk::CHUNK_DEPTH);

	position.x =
	    static_cast<float>(static_cast<int>(position.x) % Chunk::CHUNK_WIDTH);
	if (position.x < 0)
	{
		posX -= 1;
		position.x += Chunk::CHUNK_WIDTH;
	}

	position.y =
	    static_cast<float>(static_cast<int>(position.y) % Chunk::CHUNK_HEIGHT);
	if (position.y < 0)
	{
		posY -= 1;
		position.y += Chunk::CHUNK_HEIGHT;
	}

	position.z =
	    static_cast<float>(static_cast<int>(position.z) % Chunk::CHUNK_DEPTH);
	if (position.z < 0)
	{
		posZ -= 1;
		position.z += Chunk::CHUNK_DEPTH;
	}

	const math::vec3 chunkPosition =
	    math::vec3(static_cast<float>(posX * Chunk::CHUNK_WIDTH),
	               static_cast<float>(posY * Chunk::CHUNK_HEIGHT),
	               static_cast<float>(posZ * Chunk::CHUNK_DEPTH));

	for (auto& chunk : m_activeChunks)
	{
		if (chunk.getChunkPos() == chunkPosition)
		{
			return chunk.getBlockAt({
			    // "INLINE" VECTOR 3 DECLARATION
			    position.x, // x position IN the chunk, not overall
			    position.y, // y position IN the chunk, not overall
			    position.z  // z position IN the chunk, not overall
			});
		}
	}

	return BlockRegistry::get()->getFromRegistryID(
	    BlockRegistry::OUT_OF_BOUNDS_BLOCK);
}

void ChunkView::setBlockAt(math::vec3 position, BlockType* block)
{
	m_map.setBlockAt(position, block);

	int posX = static_cast<int>(position.x / Chunk::CHUNK_WIDTH);
	int posY = static_cast<int>(position.y / Chunk::CHUNK_HEIGHT);
	int posZ = static_cast<int>(position.z / Chunk::CHUNK_DEPTH);

	position.x =
	    static_cast<float>(static_cast<int>(position.x) % Chunk::CHUNK_WIDTH);
	if (position.x < 0)
	{
		posX -= 1;
		position.x += Chunk::CHUNK_WIDTH;
	}

	position.y =
	    static_cast<float>(static_cast<int>(position.y) % Chunk::CHUNK_HEIGHT);
	if (position.y < 0)
	{
		posY -= 1;
		position.y += Chunk::CHUNK_HEIGHT;
	}

	position.z =
	    static_cast<float>(static_cast<int>(position.z) % Chunk::CHUNK_DEPTH);
	if (position.z < 0)
	{
		posZ -= 1;
		position.z += Chunk::CHUNK_DEPTH;
	}

	const math::vec3 chunkPosition =
	    math::vec3(static_cast<float>(posX * Chunk::CHUNK_WIDTH),
	               static_cast<float>(posY * Chunk::CHUNK_HEIGHT),
	               static_cast<float>(posZ * Chunk::CHUNK_DEPTH));

	for (auto& chunk : m_activeChunks)
	{
		if (chunk.getChunkPos() == chunkPosition)
		{
			chunk.setBlockAt(
			    {
			        // "INLINE" VECTOR 3 DECLARATION
			        position.x, // x position IN the chunk, not overall
			        position.y, // y position IN the chunk, not overall
			        position.z  // z position IN the chunk, not overall
			    },
			    block);

			gfx::ChunkMesher mesher(chunkPosition, chunk.getBlocks(),
			                        m_renderer->getTextureTable());
			mesher.mesh();

			m_renderer->updateChunk(mesher.getMesh(), chunkPosition);

			break;
		}
	}
}
