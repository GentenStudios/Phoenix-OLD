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

#include <Common/Logger.hpp>
#include <Common/Voxels/Map.hpp>

#include <iostream>
#include <utility>

using namespace phx::voxels;

Map::Map(const std::string& save, const std::string& name)
    : m_save(save), m_mapName(name)
{
}

Map::Map(phx::BlockingQueue<Chunk>* queue) : m_queue(queue) {}

Chunk* Map::getChunk(const phx::math::vec3& pos)
{
	if (m_chunks.find(pos) != m_chunks.end())
	{
		return &m_chunks.at(pos);
	}

	if (m_queue != nullptr)
	{
		if (m_queue->empty())
		{
			return nullptr;
		}
		size_t size = m_queue->size();
		for (size_t i = 0; i < size; i++)
		{
			Chunk chunk = Chunk(math::vec3 {0, 0, 0});
			if (!m_queue->try_pop(chunk))
			{
				LOG_WARNING("CHUNK_VIEW")
				    << "Attempted to pop from empty chunk queue";
				return nullptr;
			}
			m_chunks.emplace(chunk.getChunkPos(), chunk);
		}
		return &m_chunks.at(pos);
	}

	// Chunk isn't in memory and we aren't networked, so lets create one
	std::ifstream saveFile;
	std::string   position = "." + std::to_string(int(pos.x)) + "_" +
	                       std::to_string(int(pos.y)) + "_" +
	                       std::to_string(int(pos.z));
	saveFile.open("Saves/" + m_save + "/" + m_mapName + position + ".save");

	if (saveFile)
	{
		std::string saveString;
		std::getline(saveFile, saveString);
		auto win = m_chunks.emplace(pos, Chunk(pos, saveString));
	}
	else
	{
		auto win = m_chunks.emplace(pos, Chunk(pos));
		m_chunks.at(pos).autoTestFill();
		save(pos);
	}
	return &m_chunks.at(pos);
}

std::pair<phx::math::vec3, phx::math::vec3> Map::getBlockPos(
    math::vec3 position)
{
	// This mess converts position types between world and inner chunk
	// positioning
	int posX = static_cast<int>(position.x / voxels::Chunk::CHUNK_WIDTH);
	int posY = static_cast<int>(position.y / voxels::Chunk::CHUNK_HEIGHT);
	int posZ = static_cast<int>(position.z / voxels::Chunk::CHUNK_DEPTH);

	position.x = static_cast<float>(static_cast<int>(position.x) %
	                                voxels::Chunk::CHUNK_WIDTH);
	if (position.x < 0)
	{
		posX -= 1;
		position.x += voxels::Chunk::CHUNK_WIDTH;
	}

	position.y = static_cast<float>(static_cast<int>(position.y) %
	                                voxels::Chunk::CHUNK_HEIGHT);
	if (position.y < 0)
	{
		posY -= 1;
		position.y += voxels::Chunk::CHUNK_HEIGHT;
	}

	position.z = static_cast<float>(static_cast<int>(position.z) %
	                                voxels::Chunk::CHUNK_DEPTH);
	if (position.z < 0)
	{
		posZ -= 1;
		position.z += voxels::Chunk::CHUNK_DEPTH;
	}

	const math::vec3 chunkPosition =
	    math::vec3(static_cast<float>(posX * voxels::Chunk::CHUNK_WIDTH),
	               static_cast<float>(posY * voxels::Chunk::CHUNK_HEIGHT),
	               static_cast<float>(posZ * voxels::Chunk::CHUNK_DEPTH));

	return {chunkPosition, position};
}

BlockType* Map::getBlockAt(math::vec3 position)
{
	const auto& pos = getBlockPos(position);
	return getChunk(pos.first)->getBlockAt(pos.second);
}

void Map::setBlockAt(phx::math::vec3 position, BlockType* block)
{
	const auto& pos   = getBlockPos(position);
	Chunk*      chunk = getChunk(pos.first);

	chunk->setBlockAt(pos.second, block);

	if (m_queue == nullptr)
	{
		save(pos.first);
	}
}

void Map::save(const phx::math::vec3& pos)
{
	if (m_queue != nullptr)
	{
		LOG_WARNING("MAP") << "Attempted to save while networked";
		return;
	}

	std::ofstream saveFile;
	std::string   position = "." + std::to_string(int(pos.x)) + "_" +
	                       std::to_string(int(pos.y)) + "_" +
	                       std::to_string(int(pos.z));
	saveFile.open("Saves/" + m_save + "/" + m_mapName + position + ".save");
	saveFile << m_chunks.at(pos).save();

	saveFile.close();
}
