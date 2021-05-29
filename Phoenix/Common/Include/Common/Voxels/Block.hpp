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
#include <Common/Metadata.hpp>

#include <sol/sol.hpp>

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
	using BlockCallback = sol::function;

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
		// the base BlockReferrer object implements this.
		static constexpr std::size_t UNKNOWN_BLOCK = 0;
		static constexpr std::size_t OUT_OF_BOUNDS_BLOCK = 1;
		static constexpr std::size_t AIR_BLOCK = 2;
		
	public:
		BlockType()  = default;
		~BlockType() = default;

		/// @brief The name of the block as displayed to the player.
		std::string displayName;

		/// @brief The unique id of the block, in the format of "mod:block".
		std::string id;

		/// @brief The unique *identifier* for the block, this is an internal
		/// variable.
		std::size_t uid = -1;

		/// @brief The material state of the block.
		BlockCategory category = BlockCategory::AIR;

		/// @brief If the object can be rotated horizontally
		bool rotH = false;

		/// @brief If the block can be rotated vertically, [[rotH]] must already
		/// be true
		bool rotV = false;

		/// @brief Callback for when the block is placed.
		BlockCallback onPlace;

		/// @brief Callback for when the block is broken.
		BlockCallback onBreak;

		/// @brief Callback for when the block is interacted with.
		BlockCallback onInteract;

		/// @brief comparison operator.
		bool operator==(const BlockType& rhs) const { return (id == rhs.id); }
	};

	/**
	 * @brief Container representing a single instance of a block.
	 *
	 * This holds a pointer to the universal type of the block and either the
	 * metadata for the block or a nullptr if there is none.
	 */
	struct Block
	{
		BlockType* type;
		Metadata*  metadata;
	};
} // namespace phx::voxels
