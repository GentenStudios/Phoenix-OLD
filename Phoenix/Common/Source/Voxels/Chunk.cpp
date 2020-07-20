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

using namespace phx::voxels;

Chunk::Chunk(const phx::math::vec3& chunkPos) : m_pos(chunkPos)
{
	m_blocks.reserve(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH);
}

Chunk::Chunk(const phx::math::vec3& chunkPos, const std::string& save)
    : m_pos(chunkPos)
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
	BlockType* block = BlockRegistry::get()->getFromID("core.air");
	if (m_pos.y < 16 && m_pos.y >= 0.f)
	{
		block = BlockRegistry::get()->getFromID("core.grass");
	}

	for (std::size_t i = 0; i < CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH; ++i)
	{
		m_blocks.push_back(block);
	}
}

phx::math::vec3          Chunk::getChunkPos() const { return m_pos; }
std::vector<BlockType*>& Chunk::getBlocks() { return m_blocks; }

BlockType* Chunk::getBlockAt(phx::math::vec3 position) const
{
	if (position.x < CHUNK_WIDTH && position.y < CHUNK_HEIGHT &&
	    position.z < CHUNK_DEPTH)
	{
		return m_blocks[getVectorIndex(position)];
	}

	return BlockRegistry::get()->getFromRegistryID(
	    1); // 1 is always out of bounds
}

void Chunk::setBlockAt(phx::math::vec3 position, BlockType* newBlock)
{
	if (position.x < CHUNK_WIDTH && position.y < CHUNK_HEIGHT &&
	    position.z < CHUNK_DEPTH)
	{
		m_blocks[getVectorIndex(position)] = newBlock;
	}
}

phx::Serializer& Chunk::operator&(phx::Serializer& ser)
{
	if (ser.m_mode == Serializer::Mode::WRITE)
	{
		ser& m_pos.x & m_pos.y & m_pos.z;
		for (BlockType* block : m_blocks)
		{
			size_t id = block->getRegistryID();
			ser&   id;
		}
		return ser;
	}
	else
	{
		m_blocks.clear();
		ser& m_pos.x & m_pos.y & m_pos.z;
		for (int i = 0; i < CHUNK_DEPTH * CHUNK_WIDTH * CHUNK_HEIGHT; i++)
		{
			size_t id;
			ser&   id;
			m_blocks.push_back(BlockRegistry::get()->getFromRegistryID(id));
		}
		return ser;
	}
}
