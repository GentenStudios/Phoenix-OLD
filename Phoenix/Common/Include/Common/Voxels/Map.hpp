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

#pragma once

#include <Common/Math/Math.hpp>
#include <Common/Save.hpp>
#include <Common/Utility/BlockingQueue.hpp>
#include <Common/Voxels/BlockReferrer.hpp>
#include <Common/Voxels/Chunk.hpp>

#include <cstddef>
#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace phx::voxels
{

	using ChunkData = std::pair<phx::math::vec3, std::vector<std::byte>>;

	struct MapEvent
	{
		// only one event for now, but to streamline things in the future if we
		// don't need to update a whole chunk, this will become much more
		// useful.
		enum Event
		{
			CHUNK_UPDATE
		};

		Event          type;
		voxels::Chunk* chunk;
	};

	class MapEventSubscriber
	{
	public:
		virtual void onMapEvent(const MapEvent& mapEvent) = 0;
	};

	class Map
	{
	public:
		Map(Save* save, const std::string& name,
		    voxels::BlockReferrer* referrer);
		Map(BlockingQueue<std::pair<math::vec3, std::vector<std::byte>>>* queue,
		    voxels::BlockReferrer* referrer);

		Chunk* getChunk(const math::vec3& pos);
		static std::pair<math::vec3, math::vec3> getBlockPos(
		    math::vec3 position);
		BlockType* getBlockAt(math::vec3 position);
		void       setBlockAt(math::vec3 pos, BlockType* block);
		void       save(const math::vec3& pos);

		void registerEventSubscriber(MapEventSubscriber* subscriber);

	private:
		void dispatchToSubscriber(const MapEvent& mapEvent) const;

		/**
		 * @brief Update the loaded chunks from the queue of incoming chunks.
		 */
		void updateChunkQueue();

		/*
		 * @brief Parse a save string into a chunk.
		 *
		 * @param searchView A string view of the save data to be parsed.
		 * @param chunk The chunk to be filled with the appropriate blocks.
		 * @return true if the parsing was successful, otherwise false.
		 */
		bool parseChunkSave(std::string_view searchView, Chunk &chunk);

		/*
		 * @brief Load a chunk from the save files.
		 *
		 * @param chunkPos The coordinates of the chunk.
		 * @return true if chunk was loaded from save, otherwise false.
		 */
		bool loadChunk(const phx::math::vec3 &chunkPos);


		/**
		 * @brief Create a new chunk and write a save file for it.
		 *
		 * @param chunkPos The coordinates of the chunk.
		 */
		void generateChunk(const phx::math::vec3 &chunkPos);

		/**
		 * @brief Get the save filepath for a chunk position.
		 *
		 * @param chunkPos The integer coordinates of the chunk.
		 * @return Relative path to the save directory.
		 */
		std::filesystem::path
			toSavePath(const phx::math::vec3i &chunkPos) const;

	private:
		std::unordered_map<math::vec3, Chunk, math::Vector3Hasher,
		                   math::Vector3KeyComparator>
		    m_chunks;

		BlockReferrer* m_referrer;

		Save*       m_save = nullptr;
		std::string m_mapName;

		BlockingQueue<std::pair<math::vec3, std::vector<std::byte>>>* m_queue =
		    nullptr;

		std::vector<MapEventSubscriber*> m_subscribers;
	};
} // namespace phx::voxels
