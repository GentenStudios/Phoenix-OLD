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

#include <Common/ContentLoader.hpp>
#include <Common/Voxels/BlockRegistry.hpp>

#include <algorithm>
#include <cstring>

using namespace phx::voxels;

BlockRegistry::BlockRegistry()
{
	ContentManager::get()->lua["voxel"] =
	    ContentManager::get()->lua.create_table();
	/**
	 * @addtogroup luaapi
	 *
	 * ---
	 * ---
	 * @section voxel voxel
	 * @brief The voxel API for interacting voxels
	 */
	ContentManager::get()->lua["voxel"]["block"] =
	    ContentManager::get()->lua.create_table();
	/**
	 * @addtogroup luaapi
	 *
	 * ---
	 * @subsection voxelblock voxel.block
	 * @brief Interfaces with blocks
	 */
	ContentManager::get()->lua["voxel"]["block"]["register"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection voxelblockreg voxel.block.register(luaBlock)
	     * @brief Sets the value of a setting based on its unique key
	     *
	     * @param luaBlock a table storing the data used to create a block
	     *
	     * @details The luaBlock table stores the following values: \n
	     * Required:
	     * - @b name: The display name for the block
	     * - @b id: The unique id for the block usually in the form module:block
	     *
	     * Optional:
	     * - @b category: The category of the block chosen from "Air", "Liquid",
	     * or "Solid" \n If not specified, the default is "Solid"
	     * - @b onPlace: A function that is run when the block is placed
	     * - @b onBreak: A function that is run when the block is broken
	     * - @b onInteract: A function that is run when the block is interacted
	     * with
	     * - @b textures: A table of filepaths where textures are located \n
	     * 			   Filepaths are relative to to the module directory \n
	     * 			   If only one filepath is specified, that will be used for
	     * all textures
	     *
	     * @b Example:
	     *
	     * @code {.lua}
	     * block = {}
	     * block.name = "Grass"
	     * block.id = "core:grass"
	     * block.textures = {"Assets/grass_side.png", "Assets/grass_side.png",
	     * "Assets/grass_side.png", "Assets/grass_side.png",
	     * "Assets/grass_top.png",  "Assets/dirt.png"}
	     * block.onBreak = function (position)
	     * 	print("grass broken at" + position)
	     * end
	     * voxel.block.register(block)
	     * @endcode
	     */
	    [](sol::table luaBlock) {
		    BlockType block;
		    {
			    block.displayName = luaBlock["name"];
			    block.id          = luaBlock["id"];

			    std::string category = luaBlock["category"];
			    if (category == "Air")
			    {
				    block.category = BlockCategory::AIR;
			    }
			    else if (category == "Liquid")
			    {
				    block.category = BlockCategory::LIQUID;
			    }
			    else
			    {
				    block.category = BlockCategory::SOLID;
			    }

			    if (luaBlock["onPlace"])
			    {
				    block.onPlace = luaBlock["onPlace"];
			    }

			    if (luaBlock["onBreak"])
			    {
				    block.onPlace = luaBlock["onPlace"];
			    }

			    if (luaBlock["onInteract"])
			    {
				    block.onPlace = luaBlock["onPlace"];
			    }

			    if (luaBlock["textures"])
			    {
				    std::array<std::string, 6> textures;
				    for (int i = 0; i < 6; i++)
				    {
					    std::string texture = luaBlock["textures"][i + 1];
					    if (texture.empty())
					    {
						    // If a texture is not supplied, we use the first
						    // texture in its place
						    texture = luaBlock["textures"][1];
					    }
					    textures[i] = "Modules/" +
					                  ContentManager::get()->currentMod + "/" +
					                  texture;
				    }
				    block.textures = textures;
			    }
		    }
		    BlockRegistry::get()->registerBlock(block);
	    };
}

void BlockRegistry::initialise()
{
	BlockType unknownBlock;
	unknownBlock.displayName = "Unknown Block";
	unknownBlock.id          = "core:unknown";
	unknownBlock.category    = BlockCategory::SOLID;
	unknownBlock.setAllTextures("Assets/unknown.png");
	registerBlock(unknownBlock);

	BlockType outOfBoundsBlock;
	outOfBoundsBlock.displayName = "Out Of Bounds";
	outOfBoundsBlock.id          = "core:out_of_bounds";
	outOfBoundsBlock.category    = BlockCategory::AIR;
	registerBlock(outOfBoundsBlock);
}

void BlockRegistry::registerBlock(BlockType blockInfo)
{
	if (std::find(m_blocks.begin(), m_blocks.end(), blockInfo) ==
	    m_blocks.end())
	{
		blockInfo.m_registryID = m_blocks.size();
		m_blocks.push_back(blockInfo);

		if (blockInfo.category != BlockCategory::AIR)
		{
			for (const std::string& tex : blockInfo.textures)
			{
				m_textures.addTexture(tex);
			}
		}
	}
}

BlockType* BlockRegistry::getFromID(const std::string& id)
{
	auto it = std::find_if(
	    m_blocks.begin(), m_blocks.end(), [id](const BlockType& block) {
		    return std::strcmp(block.id.c_str(), id.c_str()) == 0;
	    });

	return it == m_blocks.end() ? getFromRegistryID(0)
	                            : &(*it); // 0 is always unknown.
}

BlockType* BlockRegistry::getFromRegistryID(std::size_t registryID)
{
	if (registryID >= m_blocks.size())
	{
		return &m_blocks[0];
	}

	return &m_blocks[registryID];
}

TextureRegistry* BlockRegistry::getTextures() { return &m_textures; }
