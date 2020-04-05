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

#include <Common/Voxels/BlockRegistry.hpp>
#include <Common/Voxels/Chunk.hpp>
#include <iostream>

#include <Common/ContentLoader.hpp>

#include <Common/Voxels/WorldGenerator.hpp>

using namespace phx::voxels;
using namespace phx;

Chunk::Chunk(math::vec3 chunkPos) : m_pos(chunkPos)
{
	m_blocks.reserve(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH);
}

Chunk::Chunk(math::vec3 chunkPos, const std::string& save) : m_pos(chunkPos)
{
	std::string_view search = save;
	size_t           pos;
	while ((pos = search.find_first_of(';')) != std::string_view::npos)
	{
		std::string result;
		result = search.substr(0, pos);
		m_blocks.push_back(BlockRegistry::get()->getFromID(result));
		search.remove_prefix(pos + 1);
	}
}

std::string Chunk::save()
{
	std::string save;
	for (BlockType* block : m_blocks)
	{
		save += block->id;
		save += ";";
	}
	return save;
}

void Chunk::autoTestFill()
{
	BlockType* block = BlockRegistry::get()->getFromID("core:air");
	if (m_pos.y < 16 && m_pos.y >= 0.f)
	{
		block = BlockRegistry::get()->getFromID("core:grass");
	}

	for (std::size_t i = 0; i < CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT; ++i)
	{
		m_blocks.push_back(block);
	}

	for (std::size_t i = 0; i < CHUNK_WIDTH; ++i)
	{
		for (std::size_t j = 0; j < CHUNK_HEIGHT; ++j)
		{
			for (std::size_t k = 0; k < CHUNK_DEPTH; ++k)
			{
				//x, y, z, strength, size, octaves, persistence, height
				//32, 0.4, 4, 1, 0.1
				setBlockAt(math::vec3(i,j,k), WorldGenerator::getTerrainBlockAt(math::vec3(m_pos.x+i, m_pos.y+j, m_pos.z+k)));
			}
		}
	}
}

math::vec3               Chunk::getChunkPos() const { return m_pos; }
std::vector<BlockType*>& Chunk::getBlocks() { return m_blocks; }

BlockType* Chunk::getBlockAt(math::vec3 position) const
{
	if (position.x < CHUNK_WIDTH && position.y < CHUNK_HEIGHT &&
	    position.z < CHUNK_DEPTH)
	{
		return m_blocks[getVectorIndex(position)];
	}

	return BlockRegistry::get()->getFromRegistryID(
	    1); // 1 is always out of bounds
}

void Chunk::setBlockAt(math::vec3 position, BlockType* newBlock)
{
	if (position.x < CHUNK_WIDTH && position.y < CHUNK_HEIGHT &&
	    position.z < CHUNK_DEPTH)
	{
		m_blocks[getVectorIndex(position)] = newBlock;
	}
}


