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

#include <utility>

using namespace phx::voxels;

Map::Map(phx::Save* save, const std::string& name, BlockReferrer* referrer)
    : m_referrer(referrer), m_save(save), m_mapName(name)
{
}

Map::Map(
    phx::BlockingQueue<std::pair<phx::math::vec3, std::vector<std::byte>>>* queue,
    BlockReferrer*                                                     referrer)
    : m_referrer(referrer), m_queue(queue)
{
}

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
		
		std::size_t size = m_queue->size();
		for (std::size_t i = 0; i < size; i++)
		{
			std::pair<math::vec3, std::vector<std::byte>> data;
			if (!m_queue->try_pop(data))
			{
				LOG_WARNING("CHUNK_VIEW")
				    << "Attempted to pop from empty chunk queue";
				return nullptr;
			}

			// we have data.
			Chunk chunk(data.first, m_referrer);
			Serializer ser;
			ser.setBuffer(data.second);
			ser << chunk;
			
			m_chunks.emplace(chunk.getChunkPos(), chunk);
		}

		if (m_chunks.find(pos) != m_chunks.end())
		{
			return &m_chunks.at(pos);
		}

		return nullptr;
	}

	// Chunk isn't in memory and we aren't networked, so lets create one
	std::ifstream saveFile;
	std::string position = "." + std::to_string(static_cast<int>(pos.x)) + "_" +
	                       std::to_string(static_cast<int>(pos.y)) + "_" +
	                       std::to_string(static_cast<int>(pos.z));
	saveFile.open("Saves/" + m_save->getName() + "/" + m_mapName + position + ".save");

	if (saveFile)
	{
		std::string saveString;
		std::getline(saveFile, saveString);

		Chunk chunk(pos, m_referrer);
		Chunk::BlockList& blocks = chunk.getBlocks();

    std::string_view search = saveString;
		std::size_t      strPos = 0;
		std::size_t      i      = 0;
		while ((strPos = search.find_first_of(';')) != std::string_view::npos)
		{
			std::string result;
			result = search.substr(0, strPos);
			blocks.push_back(m_referrer->blocks.get(*m_referrer->referrer.get(result)));
			search.remove_prefix(strPos + 1);
		}

		// something went wrong if the amount of blocks is different.
		if (blocks.size() != Chunk::CHUNK_MAX_BLOCKS)
		{
			LOG_WARNING("MAP") << "Existing save for chunk at: " << pos
			                   << " is invalid, regenerating";

			blocks.clear();
			blocks.reserve(4096);
			
			BlockType* block = nullptr;
			if (chunk.getChunkPos().y >= 0)
			{
				block = m_referrer->blocks.get(
				    *m_referrer->referrer.get("core.air"));
			}
			else
			{
				block = m_referrer->blocks.get(
				    *m_referrer->referrer.get("core.grass"));
			}

			for (int i = 0; i < Chunk::CHUNK_MAX_BLOCKS; ++i)
			{
				blocks.push_back(block);
			}
		}

		m_chunks.emplace(pos, std::move(chunk));
	}
	else
	{
		// save doesn't exist, generate it.
		Chunk chunk(pos, m_referrer);

		BlockType* block = nullptr;
		if (chunk.getChunkPos().y >= 0)
		{
			block =
			    m_referrer->blocks.get(*m_referrer->referrer.get("core.air"));
		}
		else
		{
			block =
			    m_referrer->blocks.get(*m_referrer->referrer.get("core.grass"));
		}

		auto& blocks = chunk.getBlocks();
		for (int i = 0; i < Chunk::CHUNK_MAX_BLOCKS; ++i)
		{
			blocks.push_back(block);
		}

		auto& blockRef = chunk.getBlocks();
		for (std::size_t i = 0;
		     i < Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH;
		     ++i)
		{
			blockRef.push_back(block);
		}
		
		m_chunks.emplace(pos, std::move(chunk));
		save(pos);
	}
	
	return &m_chunks.at(pos);
}

std::pair<phx::math::vec3, phx::math::vec3> Map::getBlockPos(
    phx::math::vec3 position)
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

BlockType* Map::getBlockAt(phx::math::vec3 position)
{
	const auto& pos   = getBlockPos(position);
	Chunk*      chunk = getChunk(pos.first);
	if (chunk == nullptr)
	{
		return m_referrer->blocks.get(BlockType::OUT_OF_BOUNDS_BLOCK);
	}

	return chunk->getBlockAt(pos.second);
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

	dispatchToSubscriber({MapEvent::CHUNK_UPDATE, chunk});
}

void Map::save(const phx::math::vec3& pos)
{
	if (m_queue != nullptr)
	{
		LOG_WARNING("MAP") << "Attempted to save while networked";
		return;
	}

	std::ofstream saveFile;
	std::string   position = "." + std::to_string(static_cast<int>(pos.x)) + "_" +
	                       std::to_string(static_cast<int>(pos.y)) + "_" +
	                       std::to_string(static_cast<int>(pos.z));
	saveFile.open("Saves/" + m_save->getName() + "/" + m_mapName + position + ".save");

	std::string saveString;
	auto&       blocks = m_chunks.at(pos).getBlocks();
	for (auto* block : blocks)
	{
		saveString += block->id + ";";
	}

	saveFile << saveString;

	saveFile.close();
}

void Map::registerEventSubscriber(MapEventSubscriber* subscriber)
{
	auto it = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);
	if (it == m_subscribers.end())
	{
		m_subscribers.push_back(subscriber);
	}
}

void Map::dispatchToSubscriber(const MapEvent& mapEvent) const
{
	for (MapEventSubscriber* sub : m_subscribers)
	{
		sub->onMapEvent(mapEvent);
	}
}
