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

#include <Client/Graphics/BlockModel.hpp>
#include <Client/Graphics/TexturePacker.hpp>

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
			textureHandles.add(voxels::BlockType::UNKNOWN_BLOCK, {0});
			textureHandles.setUnknownReturnVal(
			    textureHandles.get(voxels::BlockType::UNKNOWN_BLOCK));

			textures.add(voxels::BlockType::UNKNOWN_BLOCK, "unknown.png");
			textures.setUnknownReturnVal(
			    textures.get(voxels::BlockType::UNKNOWN_BLOCK));

			models.add(voxels::BlockType::UNKNOWN_BLOCK,
			           gfx::BlockModel::BLOCK);
			models.setUnknownReturnVal(
			    models.get(voxels::BlockType::UNKNOWN_BLOCK));
		}

		voxels::BlockReferrer referrer;

		gfx::TexturePacker texturePacker;
		Registry<std::size_t, std::vector<gfx::TexturePacker::Handle>>
		    textureHandles;
		Registry<gfx::TexturePacker::Handle, std::string> textures;

		Registry<std::size_t, gfx::BlockModel> models;

		void registerAPI(cms::ModManager* manager)
		{
			manager->registerFunction(
			    "voxel.block.register", [manager, this](sol::table luaBlock) {
				    voxels::BlockType block;

					sol::optional<std::string> name = luaBlock["name"];
					if (name)
					{
						block.displayName = *name;
					}
					else
					{
						// log the error and return to make this a recoverable error.
					    LOG_FATAL("MODDING")
					        << "The mod at: " << manager->getCurrentModPath()
					        << " attempts to register a block without "
					        << "specifying a name.";
						return;
					}

					sol::optional<std::string> id = luaBlock["id"];
					if (id)
				    {
						block.id          = luaBlock.get<std::string>("id");
					}
					else
				    {
					    // log the error and return to make this a recoverable
					    // error.
					    LOG_FATAL("MODDING")
					        << "The mod at: " << manager->getCurrentModPath()
					        << " attempts to register a block ("
					        << block.displayName << ") without "
					        << "specifying a name.";
					    return;
					}

				    sol::optional<std::string> cat = luaBlock["category"];
				    if (cat)
				    {
						if (*cat == "Solid")
						{
							block.category = voxels::BlockCategory::SOLID;
						}
					    else if (*cat == "Liquid")
						{
							block.category = voxels::BlockCategory::LIQUID;
						}
					    else if (*cat == "Air")
					    {
						    block.category = voxels::BlockCategory::AIR;
					    }
				    }
				    else
				    {
					    // make solid by default in case none of above
					    // conditions are met.
					    block.category = voxels::BlockCategory::SOLID;
				    }

				    sol::optional<bool> rotH = luaBlock["roth"];
				    if (rotH)
				    {
					    block.rotH = *rotH;
				    }

				    sol::optional<bool> rotV = luaBlock["roth"];
				    if (rotH)
				    {
					    block.rotH = *rotV;
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

				    sol::optional<std::vector<std::string>> luaTextures = luaBlock["textures"]; // NOLINT: ugly
				    std::vector<gfx::TexturePacker::Handle> handles;
				    if (luaTextures)
				    {
					    for (const auto& tex : *luaTextures)
					    {
						    auto handle = texturePacker.add(
						        manager->getCurrentModPath() + tex);

					    	handles.push_back(handle);

					    	// don't add the whole path to the real texture names.
						    textures.add(handle, tex);
					    }
				    }

				    std::size_t blockUID   = referrer.referrer.size();
				    block.uniqueIdentifier = blockUID;

				    referrer.referrer.add(block.id, blockUID);
				    referrer.blocks.add(blockUID, block);

			    	// if handles are empty, then there clearly aren't any textures so don't waste any memory :)
			    	if (!handles.empty())
			    	{
					    textureHandles.add(blockUID, handles);
			    	}

			    	// only add model if a solid (entities will have different
				    // system, liquids and gasses will have another system too.)
			    	if (block.category == voxels::BlockCategory::SOLID)
			    	{
					    gfx::BlockModel model = gfx::BlockModel::BLOCK;

						// this is shit, lets find a better way to do this.
					    sol::optional<std::string> luaModel = luaBlock["model"];
			    		if (luaModel)
			    		{
						    if (luaModel == "Block")
						    {
							    model = gfx::BlockModel::BLOCK;
						    }
						    else if (luaModel == "Slab")
						    {
							    model = gfx::BlockModel::SLAB;
						    }
						    else if (luaModel == "Slope")
						    {
							    model = gfx::BlockModel::SLOPE;
						    }
						    else if (luaModel == "Stair")
						    {
							    model = gfx::BlockModel::STAIR;
						    }
						    else if (luaModel == "XPanel")
						    {
							    model = gfx::BlockModel::X_PANEL;
						    }
						    else if (luaModel == "XPanelCube")
						    {
							    model = gfx::BlockModel::X_PANEL_CUBE;
						    }
			    		}
			    		
					    models.add(blockUID, model);
			    	}
			    });
		}
	};
} // namespace phx::client
