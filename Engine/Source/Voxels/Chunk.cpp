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

#include <Quartz2/Voxels/Chunk.hpp>
#include <Quartz2/Voxels/BlockRegistry.hpp>

using namespace q2::voxels;
using namespace q2;

Chunk::Chunk(math::vec3 chunkPos) : m_pos(chunkPos)
{
	m_blocks.reserve(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH);
}

void Chunk::autoTestFill()
{
	BlockType* grass = BlockRegistry::instance()->getFromID("core:grass");
	for (std::size_t i = 0; i < CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH; ++i)
	{
		m_blocks.push_back(grass);
	}
}

math::vec3 Chunk::getChunkPos() const { return m_pos; }
std::vector<BlockType*>& Chunk::getBlocks() { return m_blocks; }

BlockType* Chunk::getBlockAt(math::vec3 position) const
{
	if (position.x < CHUNK_WIDTH && position.y < CHUNK_HEIGHT &&
	    position.z < CHUNK_DEPTH)
	{
		return m_blocks[getVectorIndex(position)];
	}

	return BlockRegistry::instance()->getFromRegistryID(1); // 1 is always out of bounds
}

void Chunk::setBlockAt(math::vec3 position, BlockType* newBlock)
{
	if (position.x < CHUNK_WIDTH && position.y < CHUNK_HEIGHT &&
	    position.z < CHUNK_DEPTH)
	{
		m_blocks[getVectorIndex(position)] = newBlock;
	}
}
