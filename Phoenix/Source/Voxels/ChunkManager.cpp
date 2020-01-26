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
#include <Phoenix/Voxels/BlockRegistry.hpp>
#include <Phoenix/Voxels/ChunkManager.hpp>

#include <iostream>

using namespace phx::voxels;
using namespace phx;

const int VIEW_DISTANCE = 1;

ChunkManager::ChunkManager(BlockType* defaultBlockType, unsigned seed)
    : m_seed(seed), m_defaultBlockType(defaultBlockType)
{
	// Because the whole generated view is a cube (yes it should be a sphere
	// blah blah) we are finding the length of the the cube side. We are
	// multiplying by 2 since we also generate in the "behind" area around the
	// player, the "left" area and the "right" area.
	const int chunkViewSideLength = (VIEW_DISTANCE * 2) + 1;
	const int maxVisibleChunks =
	    chunkViewSideLength * chunkViewSideLength * chunkViewSideLength;

	m_renderer = new gfx::ChunkRenderer(maxVisibleChunks);
	m_renderer->buildTextureArray();
}

void ChunkManager::tick(math::vec3 position)
{
	// turn into voxel space coordinates
	position = position / 2.f;
	position += 0.5f;

	const int posX = static_cast<int>(position.x) / Chunk::CHUNK_WIDTH;
	const int posY = static_cast<int>(position.y) / Chunk::CHUNK_HEIGHT;
	const int posZ = static_cast<int>(position.z) / Chunk::CHUNK_DEPTH;

	if (m_lastPos == math::vec3(posX, posY, posZ))
	{
		return;
	}

	m_lastPos = {posX, posY, posZ};

	m_chunks.clear();
	for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; z++)
	{
		for (int y = -VIEW_DISTANCE; y <= VIEW_DISTANCE; y++)
		{
			for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++)
			{
				math::vec3 chunkToCheck = {
				    ((static_cast<float>(x) * VIEW_DISTANCE) + posX) *
				        Chunk::CHUNK_WIDTH,
				    ((static_cast<float>(y) * VIEW_DISTANCE) + posY) *
				        Chunk::CHUNK_HEIGHT,
				    ((static_cast<float>(z) * VIEW_DISTANCE) + posZ) *
				        Chunk::CHUNK_DEPTH};

				m_chunks.emplace_back(chunkToCheck);
				m_chunks.back().autoTestFill();

				gfx::ChunkMesher mesher(chunkToCheck,
				                        m_chunks.back().getBlocks(),
				                        m_renderer->getTextureTable());
				mesher.mesh();

				// Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
				// x + CHUNK_WIDTH * (y + CHUNK_HEIGHT * z);
				const int viewSideLength = (VIEW_DISTANCE * 2) + 1;
				const int newX           = x + VIEW_DISTANCE;
				const int newY           = y + VIEW_DISTANCE;
				const int newZ           = z + VIEW_DISTANCE;
				const int index =
				    newX + viewSideLength * (newY + viewSideLength * newZ);
				std::cout << "New Chunk: " << index << std::endl;
				m_renderer->submitChunkMesh(mesher.getMesh(), index);
			}
		}
	}
}

void ChunkManager::setBlockAt(math::vec3 position, BlockType* block)
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

	for (auto& chunk : m_chunks)
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

			break;
		}
	}
}

BlockType* ChunkManager::getBlockAt(math::vec3 position) const
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

	for (auto& chunk : m_chunks)
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

void ChunkManager::breakBlockAt(math::vec3 position, BlockType* block) {}

void ChunkManager::placeBlockAt(math::vec3 position, BlockType* block) {}

void ChunkManager::render() { m_renderer->render(); }
