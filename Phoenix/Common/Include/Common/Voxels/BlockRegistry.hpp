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

#include <Common/Singleton.hpp>
#include <Common/Voxels/Block.hpp>
#include <Common/Voxels/TextureRegistry.hpp>

#include <vector>

namespace phx::voxels
{
	/**
	 * @brief A registry of all [[BlockTypes]] that have been loaded.
	 *
	 * This class is to some extent a wrapper over an std::vector. It does
	 * however, make functionality easier to use, and as a singleton it
	 * makes it a lot easier to access this data across the whole game code.
	 *
	 * @paragraph Usage
	 * @code
	 * BlockRegistry::get()->initialise(); // the get method is inherited
	 *                                     // from the Singleton base class.
	 *
	 * BlockType dirtBlock;
	 *     dirtBlock.displayName = "Air";
	 *     dirtBlock.id = "core.air";
	 *     dirtBlock.category = BlockCategory::AIR;
	 *
	 * BlockRegistry::get()->registerBlock(dirtBlock);
	 *
	 * std::cout << BlockRegistry::get()->getFromID("core.air") <<
	 * std::endl;
	 * @endcode
	 */
	class BlockRegistry : public Singleton<BlockRegistry>
	{
	public:
		/// @brief Registers respective Lua methods.
		BlockRegistry();

		// @brief Initializes the registry.
		void initialise();

		/**
		 * @brief Registers a block in the registry.
		 * @param blockInfo The blockType already put together.
		 */
		void registerBlock(BlockType blockInfo);

		/**
		 * @brief Gets a block from the registry based on its unique ID.
		 * @param id The unique ID of the block, usually in the form
		 * core.dirt.
		 * @return BlockType* A pointer to the block in the registry.
		 */
		BlockType* getFromID(const std::string& id);

		/**
		 * @brief Gets a block from the registry based on its registry ID.
		 * @note The registry ID is only used during runtime, do not try to
		 * store in a save.
		 * @param registryID The registry ID for the block.
		 * @return BlockType* A pointer to the block in the registry.
		 */
		BlockType* getFromRegistryID(std::size_t registryID);

		/**
		 * @brief Get the textures for the block.
		 *
		 * @return TextureRegistry* The registry responsible for handling
		 * block textures.
		 */
		TextureRegistry* getTextures();

		static constexpr int UNKNOWN_BLOCK       = 0;
		static constexpr int OUT_OF_BOUNDS_BLOCK = 1;

	private:
		/**
		 * @brief Stores the blockTypes in the registry.
		 *
		 * @note We used to use an std::list to prevent invalidating any
		 * pointers, however, since all blocks will be registered in ONE go
		 * from a Lua initialisation, these pointers will not be invalidated
		 * for their whole lifetime, until the block registry is destroyed -
		 * but that will be quite late in the destruction of the program so
		 * this *shouldn't* be an issue. - @beeperdeeper089
		 */
		std::vector<BlockType> m_blocks;

		/**
		 * @brief Stores the unique paths to textures.
		 *
		 */
		TextureRegistry m_textures;
	};
} // namespace phx::voxels

