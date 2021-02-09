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

#include <Common/Voxels/Chunk.hpp>

using namespace phx::voxels;

Chunk::Chunk(const phx::math::vec3& chunkPos, BlockReferrer* referrer)
    : m_pos(chunkPos), m_referrer(referrer)
{
	m_blocks.reserve(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH);
}

phx::math::vec3   Chunk::getChunkPos() const { return m_pos; }
Chunk::BlockList& Chunk::getBlocks() { return m_blocks; }

Block Chunk::getBlockAt(std::size_t index)
{
	if (index < CHUNK_MAX_BLOCKS)
	{
		if (m_metadata.find(index) != m_metadata.end())
		{
			return {m_blocks[index], &m_metadata.at(index)};
		}
		return {m_blocks[index], nullptr};
	}

	return {m_referrer->blocks.get(BlockType::OUT_OF_BOUNDS_BLOCK), nullptr};
}

Block Chunk::getBlockAt(const phx::math::vec3& position)
{
	return getBlockAt(getVectorIndex(position));
}

void Chunk::setBlockAt(const phx::math::vec3& position, Block newBlock)
{
	if (position.x < CHUNK_WIDTH && position.y < CHUNK_HEIGHT &&
	    position.z < CHUNK_DEPTH)
	{
		Block oldBlock = getBlockAt(position);
		if (oldBlock.type->onBreak)
		{
			oldBlock.type->onBreak(position.x, position.y, position.z);
		}
		m_blocks[getVectorIndex(position)] = newBlock.type;
		if (newBlock.metadata != nullptr)
		{
			m_metadata[getVectorIndex(position)] = *newBlock.metadata;
		}
		if (newBlock.type->onPlace)
		{
			newBlock.type->onPlace(position.x, position.y, position.z);
		}
	}
}

/**
 * @TODO Should we return a tuple with an error type here? There are two things
 * that could go wrong either the block is OOB or the metadata type is invalid.
 * Or should we just assert on the second error?
 */
bool Chunk::setMetadataAt(const phx::math::vec3& position,
                          const std::string& key, std::any* newData)
{
	if (position.x < CHUNK_WIDTH && position.y < CHUNK_HEIGHT &&
	    position.z < CHUNK_DEPTH)
	{
		return m_metadata[getVectorIndex(position)].set(key, newData);
	}
	return false;
}

bool Chunk::canRepeat(std::size_t i) const
{
	if (i + 1 >= CHUNK_MAX_BLOCKS)
		return false;
	if (m_blocks[i + 1]->id != m_blocks[i]->id)
		return false;
	if (m_metadata.find(i + 1) != m_metadata.end())
		return false;
	return true;
};

phx::Serializer& Chunk::operator>>(phx::Serializer& ser) const
{

	ser << m_pos.x << m_pos.y << m_pos.z;
	for (int i = 0; i < CHUNK_MAX_BLOCKS; i++)
	// for (const BlockType* block : m_blocks)
	{
		ser << m_blocks[i]->id;
		if (m_metadata.find(i) != m_metadata.end())
		{
			ser << '+' << m_metadata.at(i);
		}
		else if (canRepeat(i))
		{
			std::size_t j = i;
			while (canRepeat(i))
			{
				i++;
			}
			ser << '*' << i - j;
		}
		else
		{
			ser << ';';
		}
	}
	return ser;
}

phx::Serializer& Chunk::operator<<(phx::Serializer& ser)
{
	m_blocks.clear();
	m_blocks.reserve(4096);

	ser >> m_pos.x >> m_pos.y >> m_pos.z;
	for (int i = 0; i < CHUNK_MAX_BLOCKS; i++)
	{
		std::string id;
		ser >> id;
		m_blocks.push_back(
		    m_referrer->blocks.get(*m_referrer->referrer.get(id)));
		char c;
		ser >> c;
		if (c == ';')
		{
		}
		else if (c == '+')
		{
			Metadata data;
			ser >> data;
			m_metadata[i] = data;
		}
		else if (c == '*')
		{
			std::size_t rep;
			ser >> rep;
			for (std::size_t j = 0; j < rep; j++)
			{
				m_blocks.push_back(
				    m_referrer->blocks.get(*m_referrer->referrer.get(id)));
				i++;
			}
		}
	}

	return ser;
}
