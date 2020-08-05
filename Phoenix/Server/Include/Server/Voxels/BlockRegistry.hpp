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
 * @file Commander.hpp
 * @brief Header file to implement a command execution system designed to
 * interface with a terminal.
 *
 * @copyright Copyright (c) 2019-2020
 *
 */

#pragma once

#include <Common/Voxels/BlockReferrer.hpp>
#include <Common/CMS/ModManager.hpp>

namespace phx::server
{
	struct BlockRegistry
	{
		BlockRegistry()
		{
			// this will be 0, which is equivalent to BlockType::UNKNOWN_BLOCK
			auto              uid = referrer.referrer.size();
			voxels::BlockType unknown;
			unknown.displayName = "Unknown Block";
			unknown.id          = "core:unknown";
			unknown.category    = voxels::BlockCategory::SOLID;
			unknown.uniqueIdentifier = uid;
			referrer.referrer.add(unknown.id, uid);
			referrer.blocks.add(uid, unknown);

			// this will be 1, which is equivalent to
			// BlockType::OUT_OF_BOUNDS_BLOCK.
			uid = referrer.referrer.size();
			voxels::BlockType outOfBounds;
			outOfBounds.displayName = "Out of Bounds";
			outOfBounds.id          = "core:out_ouf_bounds";
			outOfBounds.category    = voxels::BlockCategory::AIR;
			outOfBounds.uniqueIdentifier = uid;
			referrer.referrer.add(outOfBounds.id, uid);
			referrer.blocks.add(uid, outOfBounds);

			// this will be 2, which is equivalent to BlockType::AIR_BLOCK.
			uid = referrer.referrer.size();
			voxels::BlockType air;
			air.displayName = "Air";
			air.id          = "core:air";
			air.category    = voxels::BlockCategory::AIR;
			air.uniqueIdentifier = uid;
			referrer.referrer.add(air.id, uid);
			referrer.blocks.add(uid, air);
		}
		
		voxels::BlockReferrer referrer;

		inline void registerAPI(cms::ModManager* manager)
		{
			manager->registerFunction(
			    "voxel.block.register",
			    [manager, this](sol::table luaBlock) {
				    voxels::BlockType block;

				    block.displayName = luaBlock["name"];
				    block.id          = luaBlock["id"];

				    const std::string category = luaBlock["category"];
				    if (category == "Solid")
				    {
					    // put solid first since that's most likely.
					    block.category = voxels::BlockCategory::SOLID;
				    }
				    else if (category == "Liquid")
				    {
					    block.category = voxels::BlockCategory::LIQUID;
				    }
				    else
				    {
					    // default to air if not liquid or solid.
					    block.category = voxels::BlockCategory::AIR;
				    }

				    sol::optional<sol::function> onPlace = luaBlock["onPlace"];
				    if (onPlace)
				    {
					    block.onPlace = *onPlace;
				    }

				    sol::optional<sol::function> onBreak = luaBlock["onBreak"];
				    if (onBreak)
				    {
					    block.onBreak = *onBreak;
				    }

					// we ignore textures on here since the server could not
				    // give a damn about that.
			    	
				    sol::optional<sol::function> onInteract =
				        luaBlock["onInteract"];
				    if (onInteract)
				    {
					    block.onInteract = *onInteract;
				    }

				    std::size_t blockUID   = referrer.referrer.size();
				    block.uniqueIdentifier = blockUID;

				    referrer.referrer.add(block.id, blockUID);
				    referrer.blocks.add(blockUID, block);
			    });
		}
	};
} // namespace phx::server
