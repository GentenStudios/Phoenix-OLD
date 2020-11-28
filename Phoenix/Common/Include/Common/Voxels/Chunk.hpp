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

#include <Common/CoreIntrinsics.hpp>
#include <Common/Math/Math.hpp>
#include <Common/Voxels/Block.hpp>
#include <Common/Voxels/BlockReferrer.hpp>
#include <Common/Registry.hpp>
#include <Common/Metadata.hpp>

#include <Common/Utility/Serializer.hpp>
#include <vector>

namespace phx::voxels
{
	/**
	 * @brief Stores the index - english relations for block faces.
	 */
	enum class BlockFace : int
	{
		FRONT  = 0,
		LEFT   = 1,
		BACK   = 2,
		RIGHT  = 3,
		TOP    = 4,
		BOTTOM = 5,
	};

	/**
	 * @brief Represents a collection of blocks in a specific area.
	 *
	 * This class represents a 16x16x16 area of blocks, with its own
	 * position in the world - obviously being a multiple of 16 in all
	 * directions.
	 *
	 * @paragraph Usage
	 * @code
	 * Chunk chunk = Chunk(math::vec3(0, 0, 0));
	 * Chunk chunk2 = Chunk(math::vec3(16, 16, 16));
	 *
	 * // usage with renderer
	 * ChunkRenderer* renderer = new ChunkRenderer(2); // view distance of 3.
	 * renderer->buildTextureArray(); // make sure all blocks are loaded in
	 * before this.
	 *
	 * ChunkMesher mesher(chunk.getChunkPos(), chunk,
	 * renderer->getTextureTable()); mesher.mesh();
	 *
	 * renderer->submitChunk(mesher.getMesh(), chunk.getChunkPos());
	 * //renderer->updateChunk(mesher.getMesh(), chunk.getChunkPos());
	 * //renderer->dropChunk(chunk.getChunkPos());
	 * @endcode
	 */
	class Chunk : public ISerializable
	{
	public:
		using BlockList = std::vector<BlockType*>;
		
	public:
		Chunk() = delete;

		Chunk(const math::vec3& chunkPos, BlockReferrer* referrer);
		~Chunk()                  = default;
		Chunk(const Chunk& other) = default;
		Chunk& operator=(const Chunk& other) = default;
		Chunk(Chunk&& other) noexcept        = default;
		Chunk& operator=(Chunk&& other) noexcept = default;

		/**
		 * @brief Get the position of the chunk.
		 * @return math::vec3 The position of the chunk.
		 */
		math::vec3 getChunkPos() const;

		/**
		 * @brief Get a vector of pointers to all the blocks in the chunk.
		 * @return std::vector<BlockType*>& Vector of pointers to all the
		 * blocks in the chunk.
		 */
		BlockList& getBlocks();

		/**
		 * @brief Gets the Block at the supplied position.
		 * @param index flattened location of the block in the chunk.
		 * @return Block The requested block.
		 */
		Block getBlockAt(std::size_t index);

		/**
		 * @brief Gets the Block at the supplied position.
		 * @param position Position of the block relative to the chunk.
		 * @return Block The requested block.
		 */
		Block getBlockAt(math::vec3 position);

		/**
		 * @brief Sets the Block at the supplied position.
		 * @param position Position of the block relative to the chunk.
		 * @param newBlock The block that now exists at this location.
		 */
		void setBlockAt(math::vec3 position, Block newBlock);

		/**
		 * @brief Sets metadata for the Block at the supplied position.
		 * @param position Position of the block relative to the chunk.
		 * @param key The key associated with the metadata.
		 * @param newData The new value for the data.
		 * @return true if the data was set.
		 * @return false if the data already exists with a different data type
		 * OR if the supplied position is out of bounds for the chunk.
		 */
		bool setMetadataAt(phx::math::vec3 position, const std::string& key,
		                   std::any* newData);

		/// @brief How wide a chunk is (x axis).
		static constexpr int CHUNK_WIDTH = 16;

		/// @brief How tall a chunk is (y axis).
		static constexpr int CHUNK_HEIGHT = 16;

		/// @brief How deep a chunk is (z axis).
		static constexpr int CHUNK_DEPTH = 16;

		/// @brief The amount of blocks in a chunk.
		static constexpr int CHUNK_MAX_BLOCKS =
		    CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH;

		/**
		 * @brief Get the Index based coordinates in a chunk.
		 *
		 * @todo Depreciate single use code in favor of the function built
		 * into the vec3 object.
		 *
		 * @param x The X coordinates of the position.
		 * @param y The Y coordinates of the position.
		 * @param z The Z coordinates of the position.
		 * @return std::size_t The flattened index position in the chunk.
		 */
		static std::size_t getVectorIndex(std::size_t x, std::size_t y,
		                                  std::size_t z)
		{
			return x + CHUNK_WIDTH * (y + CHUNK_HEIGHT * z);
		}

		/**
		 * @brief Calculates the position in which a 3D position would be in
		 * a 1D array.
		 *
		 * @param pos The 3-dimensional position of the data.
		 * @return The position it would be in a 1D array.
		 */
		ENGINE_FORCE_INLINE static std::size_t getVectorIndex(math::vec3 pos)
		{
			return getVectorIndex(static_cast<std::size_t>(pos.x),
			                      static_cast<std::size_t>(pos.y),
			                      static_cast<std::size_t>(pos.z));
		}

		// serialize.
		Serializer& operator>>(Serializer& ser) const override;

		// deserialize.
		Serializer& operator<<(Serializer& ser) override;

	private:
		math::vec3                                m_pos;
		BlockList                                 m_blocks;
		std::unordered_map<std::size_t, Metadata> m_metadata;
		BlockReferrer*                            m_referrer;
	};
} // namespace phx::voxels
