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

ChunkView::ChunkView(int viewDistance, Map* map)
    : m_viewDistance(viewDistance), m_map(map)
{
	// calculates the maximum visible chunks.
	const int viewLength       = (viewDistance * 2) + 1;
	const int maxVisibleChunks = viewLength * viewLength * viewLength;

	m_renderer = new gfx::ChunkRenderer(maxVisibleChunks);
	m_renderer->buildTextureArray();
}

ChunkView::ChunkView(int viewDistance, client::Network* network)
    : m_viewDistance(viewDistance), m_network(network)
{
	// calculates the maximum visible chunks.
	const int viewLength       = (viewDistance * 2) + 1;
	const int maxVisibleChunks = viewLength * viewLength * viewLength;

	m_renderer = new gfx::ChunkRenderer(maxVisibleChunks);
	m_renderer->buildTextureArray();
}

ChunkView::~ChunkView()
{
	delete m_renderer;
	delete m_map;
}

void ChunkView::tick(math::vec3 playerPos)
{
	// When networked, we only tick if we have new chunks from the network
	if (m_network != nullptr)
	{
		if (m_network->chunkQueue.empty())
		{
			return;
		}
	}

	// this converts the raw camera/player position into voxel-world positions.
	playerPos = (playerPos / 2.f) + 0.5f;

	// Use alternate tick function if we are connected to a server
	if (m_network != nullptr)
	{
		tickNet(playerPos);
	}
	tickLocal(playerPos);
}

void ChunkView::tickNet(math::vec3 playerPos)
{
	// TODO discard all out of view chunks

	// If we don't already have a chunk, insert it
	std::size_t size = m_network->chunkQueue.size();
	for (std::size_t i = 0; i < size; i++)
	{
		Chunk chunk = Chunk(math::vec3 {});
		if (!m_network->chunkQueue.try_pop(chunk))
		{
			LOG_WARNING("CHUNK_VIEW")
			    << "Attempted to pop from empty chunk queue";
			return;
		}
		math::vec3 chunkToCheck = chunk.getChunkPos();

		chunkToCheck = chunkToCheck * static_cast<float>(Chunk::CHUNK_WIDTH);

		auto result = std::find_if(m_activeChunks.begin(), m_activeChunks.end(),
		                           [chunkToCheck](const Chunk& o) -> bool {
			                           return o.getChunkPos() == chunkToCheck;
		                           });

		if (result == m_activeChunks.end())
		{
			m_activeChunks.emplace_back(chunk);

			gfx::ChunkMesher mesher(chunkToCheck,
			                        m_activeChunks.back().getBlocks(),
			                        m_renderer->getTextureTable());

			mesher.mesh();

			m_renderer->submitChunk(mesher.getMesh(), chunkToCheck);
		}
	}
}

void ChunkView::tickLocal(math::vec3 playerPos)
{
	const int posX = static_cast<int>(playerPos.x) / Chunk::CHUNK_WIDTH;
	const int posY = static_cast<int>(playerPos.y) / Chunk::CHUNK_HEIGHT;
	const int posZ = static_cast<int>(playerPos.z) / Chunk::CHUNK_DEPTH;

	for (int x = -m_viewDistance; x <= m_viewDistance; x++)
	{
		for (int y = -m_viewDistance; y <= m_viewDistance; y++)
		{
			for (int z = -m_viewDistance; z <= m_viewDistance; z++)
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
					m_activeChunks.emplace_back(m_map->getChunk(chunkToCheck));

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
	if (m_network != nullptr)
	{
		// TODO add block manipulation over network
		LOG_WARNING("ChunkView.cpp")
		    << "Block manipulation when networked, not currently supported";
		return;
	}
	else
	{
		m_map->setBlockAt(position, block);
	}

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
