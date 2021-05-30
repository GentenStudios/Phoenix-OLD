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

#include <Common/Registry.hpp>
#include <Common/Voxels/Item.hpp>

#include <string>

namespace phx::voxels
{
	/**
	 * @brief Acts as a universal pairing between string and item.
	 *
	 * This class is a universal struct that can be passed around to act as a
	 * minimal set of data.
	 *
	 * Each application implements their own Registry with a few more pieces
	 * of data and lua registration methods.
	 */
	struct ItemReferrer
	{
		/**
		 * @brief Automatically implements the Unknown, Out of Bounds and Air
		 * blocks as necessary.
		 */
		ItemReferrer()
		{
			// this will be 0, which is equivalent to ItemType::UNKNOWN_ITEM
			auto             uid = referrer.size();
			voxels::ItemType unknown;
			unknown.displayName      = "Unknown Item";
			unknown.id               = "core.unknown";
			unknown.uid              = uid;
			referrer.add(unknown.id, uid);
			items.add(uid, unknown);

			// this will be 1, which is equivalent to
			// ItemType::OUT_OF_BOUNDS_ITEM.
			uid = referrer.size();
			voxels::ItemType outOfBounds;
			outOfBounds.displayName      = "Out of Bounds";
			outOfBounds.id               = "core.out_of_bounds";
			outOfBounds.uid              = uid;
			referrer.add(outOfBounds.id, uid);
			items.add(uid, outOfBounds);

			// by default will return unknown blocks if they don't exist.
			// a tiny bit hacky but solves a lot of problems without writing a
			// bunch of unnecessary code.
			referrer.setUnknownReturnVal(referrer.get("core.unknown"));
			items.setUnknownReturnVal(
			    items.get(voxels::ItemType::UNKNOWN_ITEM));
		}

		// referrer refers a string to int, which in turn is used to get the
		// item type.
		Registry<std::string, std::size_t> referrer;
		Registry<std::size_t, ItemType>    items;
	};
} // namespace phx::voxels
