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
#include <Common/Math/Math.hpp>
#include <Common/Save.hpp>
#include <Common/Utility/Serializer.hpp>
#include <Common/Voxels/Map.hpp>

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

using namespace phx::voxels;

Map::Map(phx::Save* save, const std::string& name, BlockReferrer* referrer)
    : m_referrer(referrer), m_save(save), m_mapName(name)
{
}

Map::Map(phx::BlockingQueue<std::pair<phx::math::vec3, std::vector<std::byte>>>*
                        queue,
         BlockReferrer* referrer)
    : m_referrer(referrer), m_queue(queue)
{
}

/*
    Chunks are kept in an unordered map of vec3 : Chunk. The algorithm looks
    first in the map. The next behavior depends on whether we are in online or
    offline mode. If we are networked, it updates the map from the queue of
    chunks from the server, then if it still isn't found, nullptr is returned.
    If we are not networked, the chunk is loaded from the save files. If the
    save file does not exist yet or is damaged, a new chunk will be generated.
*/
Chunk* Map::getChunk(const phx::math::vec3& pos)
{
	if (m_chunks.find(pos) != m_chunks.end())
	{
		return &m_chunks.at(pos);
	}

	if (m_queue)
	{
		updateChunkQueue();
		if (m_chunks.find(pos) != m_chunks.end())
		{
			return &m_chunks.at(pos);
		}

		return nullptr;
	}

	// Chunk isn't in memory and we aren't networked, so lets create one
	if (!loadChunk(pos))
	{
		// save doesn't exist, generate it.
		generateChunk(pos);
	}

	return &m_chunks.at(pos);
}

std::pair<phx::math::vec3, phx::math::vec3> Map::getBlockPos(
    phx::math::vec3 position)
{
	// This mess converts position types between world and inner chunk
	// positioning
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
	saveFile.open(toSavePath(static_cast<phx::math::vec3i>(pos)));

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

void Map::updateChunkQueue()
{
	if (!m_queue || m_queue->empty())
	{
		return;
	}

	// BlockingQueue methods have mutex overhead, so we minimize method calls.
	// The size of the queue changes during iteration.
	const std::size_t chunksQueued = m_queue->size();
	for (std::size_t i = 0; i < chunksQueued; ++i)
	{
		ChunkData data;
		if (!m_queue->try_pop(data))
		{
			LOG_WARNING("CHUNK_VIEW")
			    << "Attempted to pop from empty chunk queue";
			break;
		}

		// We have chunk data.
		Chunk           chunk {data.first, m_referrer};
		phx::Serializer ser;
		ser.setBuffer(data.second);
		ser << chunk;

		m_chunks.emplace(chunk.getChunkPos(), chunk);
	}

	return;
}

bool Map::parseChunkSave(std::string_view searchView, Chunk& chunk)
{
	// We will add blocks to the chunk as they are parsed.
	Chunk::BlockList& blocks = chunk.getBlocks();

	for (std::size_t delimiterPos = searchView.find_first_of(';');
	     delimiterPos != std::string_view::npos;
	     searchView.remove_prefix(delimiterPos + 1),
	                 delimiterPos = searchView.find_first_of(';'))
	{
		const std::string blockName =
		    static_cast<std::string>(searchView.substr(0, delimiterPos));
		blocks.push_back(
		    m_referrer->blocks.get(*m_referrer->referrer.get(blockName)));
	}

	if (blocks.size() != Chunk::CHUNK_MAX_BLOCKS)
	{
		LOG_WARNING("MAP") << "Existing save for chunk at: "
		                   << chunk.getChunkPos() << " is invalid";
		return false;
	}

	return true;
}

bool Map::loadChunk(const phx::math::vec3& chunkPos)
{
	std::ifstream saveFile;
	saveFile.open(toSavePath(static_cast<phx::math::vec3i>(chunkPos)));

	if (!saveFile)
	{
		// File could not be opened.
		return false;
	}

	std::string saveString;
	std::getline(saveFile, saveString);

	Chunk chunk {chunkPos, m_referrer};

	if (!parseChunkSave(saveString, chunk))
	{
		return false;
	}

	m_chunks.emplace(chunkPos, std::move(chunk));
	return true;
}

// Creates a new chunk and fills it with either grass or air, depending on its
// position on the y axis. If it is below y = 0, it will be grass. Otherwise
// air will be generated.
void Map::generateChunk(const phx::math::vec3& chunkPos)
{
	BlockType* fillBlock {};
	// Position type needs to be converted.
	if (chunkPos.y >= 0)
	{
		fillBlock =
		    m_referrer->blocks.get(*m_referrer->referrer.get("core.air"));
	}
	else
	{
		fillBlock =
		    m_referrer->blocks.get(*m_referrer->referrer.get("core.grass"));
	}

	Chunk             chunk {chunkPos, m_referrer};
	Chunk::BlockList& blocks = chunk.getBlocks();
	for (std::size_t i = 0; i < Chunk::CHUNK_MAX_BLOCKS; ++i)
	{
		blocks.push_back(fillBlock);
	}

	m_chunks.emplace(chunkPos, std::move(chunk));

	save(chunkPos);
}

std::filesystem::path Map::toSavePath(const phx::math::vec3i& chunkPos) const
{
	const std::string posString = std::to_string(chunkPos.x) + '_' +
	                              std::to_string(chunkPos.y) + '_' +
	                              std::to_string(chunkPos.z);

	const std::filesystem::path savePath = phx::saveDir + m_save->getName() +
	                                       '/' + m_mapName + '.' + posString +
	                                       ".save";

	return savePath;
}
