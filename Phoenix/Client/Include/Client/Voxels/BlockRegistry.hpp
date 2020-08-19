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

#include <Common/CMS/ModManager.hpp>
#include <Common/Registry.hpp>
#include <Common/Voxels/BlockReferrer.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace phx::client
{
	/**
	 * @brief Acts as a register for block data throughout the client
	 * application.
	 *
	 * This holds a block referrer which handles default initialized blocks and
	 * then provides an API registration for registering blocks from within Lua.
	 *
	 * We implemented this instead of the original impl because something like
	 * the server does not need to store things like texture paths - which is
	 * quite essential on the client.
	 */
	struct BlockRegistry
	{
		BlockRegistry()
		{
			textures.add(voxels::BlockType::UNKNOWN_BLOCK,
			             {"Assets/unknown.png"});
			textures.setUnknownReturnVal(
			    textures.get(voxels::BlockType::UNKNOWN_BLOCK));
		}

		voxels::BlockReferrer referrer;

		Registry<std::size_t, std::vector<std::string>> textures;

		void registerAPI(cms::ModManager* manager)
		{
			manager->registerFunction(
			    "voxel.block.register", [manager, this](sol::table luaBlock) {
				    voxels::BlockType block;

				    block.displayName = luaBlock["name"];
				    block.id          = luaBlock["id"];

				    const std::string category = luaBlock["category"];
				    if (category == "Air")
				    {
					    // put solid first since that's most likely.
					    block.category = voxels::BlockCategory::AIR;
				    }
				    else if (category == "Liquid")
				    {
					    block.category = voxels::BlockCategory::LIQUID;
				    }
				    else
				    {
					    // default to solid if not liquid or air.
					    block.category = voxels::BlockCategory::SOLID;
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

				    sol::optional<sol::function> onInteract =
				        luaBlock["onInteract"];
				    if (onInteract)
				    {
					    block.onInteract = *onInteract;
				    }

				    bool                                    setTex = false;
				    sol::optional<std::vector<std::string>> luaTextures =
				        luaBlock["textures"];
				    if (luaTextures)
				    {
					    for (auto& tex : *luaTextures)
					    {
						    // we have to do this so it's pointing to the right
						    // directory.
						    tex = manager->getCurrentModPath() + tex;
					    }

					    setTex = true;
				    }

				    std::size_t blockUID   = referrer.referrer.size();
				    block.uniqueIdentifier = blockUID;

				    referrer.referrer.add(block.id, blockUID);
				    referrer.blocks.add(blockUID, block);
				    if (setTex)
				    {
					    textures.add(blockUID, *luaTextures);
				    }
			    });
		}
	};
} // namespace phx::client
