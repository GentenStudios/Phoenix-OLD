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

/**
 * @file Block.hpp
 * @brief The basic block type object
 *
 * @copyright Copyright (c) Genten Studios 2019 - 2020
 *
 */

#pragma once

#include <Common/Math/Math.hpp>

#include <array>
#include <functional>
#include <string>

namespace phx::voxels
{
	/**
	 * @brief The material state of the block.
	 *
	 * A block exist in one of three states, either "SOLID", "AIR", or a
	 * "LIQUID". These states define if the player can walk through the
	 * block or not in addition to other potential information (like if
	 * the object flows).
	 */
	enum class BlockCategory : unsigned int
	{
		AIR,
		SOLID,
		LIQUID
	};

	/**
	 * @brief A callback function for when actions related to a block occur.
	 *
	 * This "typedef" is just a QoL thing, it's much better than typing out
	 * the whole definition each and every time. HOWEVER, this is also since
	 * this is VERY likely to change as we further develop implementations
	 * of this feature.
	 */
	using BlockCallback = std::function<void(math::vec3 pos)>;

	/**
	 * @brief The universal data for blocks.
	 *
	 * This stores any information that is consistent of blocks of the same
	 * "Type" regardless of the number of instances of that block in the
	 * world. For example a dirt block will have the same basic information
	 * every time it exists in the world.
	 */
	class BlockType
	{
	public:
		BlockType()  = default;
		~BlockType() = default;

		/// @brief The name of the block as displayed to the player.
		std::string displayName;

		/// @brief The unique id of the block, in the format of "mod:block".
		std::string id;

		/// @brief The material state of the block.
		BlockCategory category = BlockCategory::AIR;

		/// @brief Callback for when the block is placed.
		BlockCallback onPlace;

		/// @brief Callback for when the block is broken.
		BlockCallback onBreak;

		/// @brief Callback for when the block is interacted with.
		BlockCallback onInteract;

		/// @brief (temporary) a bitpacked color: rrrrggggbbbbaaaa (a letter = 1 bit)
		unsigned color = 0b1111111111111111;

		/**
		 * @brief An array of texture paths used for rendering the block.
		 *
		 * The textures are in the order front, left, back, right, top,
		 * bottom.
		 */
		std::array<std::string, 6> textures;

		/**
		 * @brief Sets all the textures to be the same thing.
		 * @param tex Path to the texture to be used.
		 */
		void setAllTextures(const std::string& tex) { textures.fill(tex); }

		/**
		 * @brief Gets the Registry ID of the blockType.
		 *
		 * The registry ID is a runtime specific int that allows for faster.
		 * access but is not preserved between runs.
		 *
		 * @return std::size_t the ID of the blockType.
		 */
		std::size_t getRegistryID() const { return m_registryID; }

		bool operator==(const BlockType& rhs) const { return (id == rhs.id); }

	private:
		std::size_t m_registryID = -1;
		friend class BlockRegistry;
	};

	/**
	 * @brief A Block's Metadata.
	 *
	 * This class is used in conjunction with the Chunk class - well, it
	 * isn't used right now, but it's here as a placeholder to make specific
	 * methods easier to handle and "future-proof". This stores the block
	 * position as it allows for storage in a SEPARATE container to allow
	 * for just "on-demand" metadata rather than metadata for all blocks -
	 * since some would just not need it.
	 */
	struct BlockMetadata
	{
		/**
		 * @brief The position of the block in the world.
		 */
		math::vec3 blockPos;

		/**
		 * @brief serialized data, we can decide how we wanna do this some
		 * other time. (just an implementation attempt to stop pains later in
		 * life)
		 */
		std::string data;
	};
} // namespace phx::voxels
