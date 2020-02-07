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

#pragma once

#include <Phoenix/CoreIntrinsics.hpp>
#include <Phoenix/Math/Math.hpp>
#include <Phoenix/Voxels/Block.hpp>

#include <vector>

namespace phx
{
	namespace voxels
	{
		/**
		 * @brief Stores the index - english relations for block faces
		 * 
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
		 * @brief Stores a group of blocks that are loaded or unloaded as the player moved. By storing blocks in a chunk we aren't loading the entire map at once.
		 * 
		 */
		class Chunk
		{
		public:
			Chunk() = delete;

			explicit Chunk(math::vec3 chunkPos);
			~Chunk()                  = default;
			Chunk(const Chunk& other) = default;
			Chunk& operator=(const Chunk& other) = default;
			Chunk(Chunk&& other) noexcept        = default;
			Chunk& operator=(Chunk&& other) noexcept = default;

			/**
			 * @brief Quick function to create solid chunks of grass
			 * 
			 * @todo Replace this with MapGen
			 */
			void autoTestFill();

			/**
			 * @brief Get the position of the chunk
			 * 
			 * @return math::vec3 The poisition of the chunk
			 */
			math::vec3               getChunkPos() const;
			/**
			 * @brief Get a vector of pointers to all the blocks in the chunk
			 * 
			 * @return std::vector<BlockType*>& Vector of pointers to all the blocks in the chunk
			 */
			std::vector<BlockType*>& getBlocks();

			/**
			 * @brief Gets the Block at the supplied position
			 * 
			 * @param position Position of the desired block relative to the chunk
			 * @return BlockType* The requested block
			 */
			BlockType* getBlockAt(math::vec3 position) const;

			/**
			 * @brief Sets the Block At the supplied position
			 * 
			 * @param position Position of the block relative to the chunk
			 * @param newBlock The block that exists at this location
			 */
			void       setBlockAt(math::vec3 position, BlockType* newBlock);

			/// @brief How wide a chunk is (x axis)
			static constexpr int CHUNK_WIDTH  = 16;
			/// @brief How tall a chunk is (y axis)
			static constexpr int CHUNK_HEIGHT = 16;
			/// @brief How deep a chunk is (z axis)
			static constexpr int CHUNK_DEPTH  = 16;

			/**
			 * @brief Get the Index based coordiantes in a chunk
			 * 
			 * @todo Depreciate single use code in favor of the function built into the vec3 object
			 * 
			 * @param x The X coordiante of the position
			 * @param y The Y coordiante of the position
			 * @param z The Z coordiante of the position
			 * @return std::size_t The flattened index position in the chunk
			 */
			static std::size_t getVectorIndex(std::size_t x, std::size_t y,
			                                  std::size_t z)
			{
				return x + CHUNK_WIDTH * (y + CHUNK_HEIGHT * z);
			}

			/**
			 * @brief No clue, Beeper?
			 * 
			 * @param pos 
			 * @return ENGINE_FORCE_INLINE getVectorIndex 
			 */
			ENGINE_FORCE_INLINE static std::size_t getVectorIndex(
			    math::vec3 pos)
			{
				return getVectorIndex(static_cast<std::size_t>(pos.x),
				                      static_cast<std::size_t>(pos.y),
				                      static_cast<std::size_t>(pos.z));
			}

		private:
			/// @brief The position of the chunk in relation to the map
			math::vec3              m_pos;
			/// @brief The blocks in the chunk
			std::vector<BlockType*> m_blocks;
		};
	} // namespace voxels
} // namespace q2
