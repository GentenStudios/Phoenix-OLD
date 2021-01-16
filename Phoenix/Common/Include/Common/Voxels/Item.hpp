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

#include <sol/sol.hpp>

#include <Common/Metadata.hpp>
#include <array>
#include <functional>
#include <string>

namespace phx::voxels
{
	/**
	 * @brief A callback function for when actions related to an Item occur.
	 *
	 * This "typedef" is just a QoL thing, it's much better than typing out
	 * the whole definition each and every time. HOWEVER, this is also since
	 * this is VERY likely to change as we further develop implementations
	 * of this feature.
	 */
	using ItemCallback = sol::function;

	/**
	 * @brief The universal data for items.
	 *
	 * This stores any information that is consistent of items of the same
	 * "Type" regardless of the number of instances of that item in the
	 * world. For example a dirt block will have the same basic information
	 * every time it exists in the world.
	 */
	struct ItemType
	{
	public:
		// the base ItemReferrer object implements this.
		static constexpr std::size_t UNKNOWN_ITEM = 0;
		static constexpr std::size_t INVALID_ITEM = 1;

	public:
		ItemType()  = default;
		~ItemType() = default;

		/// @brief The name of the item as displayed to the player.
		std::string displayName;

		/// @brief The unique id of the item, in the format of "mod.item".
		std::string id;

		/// @brief The unique *identifier* for the item, this is an internal
		/// variable.
		std::size_t uniqueIdentifier = -1;

		/**
		 * @brief The block that is placed when onPlace is called. If left
		 * undefined, no block is placed.
		 *
		 * @note onPlace is still called regardless of if this is defined
		 */
		std::string places;

		/// @brief Callback for when the secondary interaction button is called.
		ItemCallback onPlace;

		/// @brief Callback for when the primary interaction is called.
		ItemCallback onInteract;
	};

	/**
	 * @brief Container representing a single instance of an Item.
	 *
	 * This holds a pointer to the universal type of the item and either the
	 * metadata for the item or a nullptr if there is none.
	 */
	struct Item
	{
		ItemType* type;
		Metadata* metadata;
	};
} // namespace phx::voxels