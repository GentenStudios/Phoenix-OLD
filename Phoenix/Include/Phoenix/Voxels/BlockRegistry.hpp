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

#include <Phoenix/Singleton.hpp>
#include <Phoenix/Voxels/Block.hpp>
#include <Phoenix/Voxels/TextureRegistry.hpp>

#include <vector>

namespace phx
{
	namespace voxels
	{
		class BlockRegistry : public Singleton<BlockRegistry>
		{
		public:
			void initialise();

			void       registerBlock(BlockType blockInfo);
			BlockType* getFromID(const std::string& id);

			// registry int is stored in the block, it's a quicker way of
			// getting a block's data. do NOT store this in chunk data, that is
			// only valid once the registry table is built.
			BlockType* getFromRegistryID(std::size_t registryID);

			TextureRegistry* getTextures();

			static constexpr int UNKNOWN_BLOCK = 0;
			static constexpr int OUT_OF_BOUNDS_BLOCK = 1;

		private:
			// NOTE: We used to use an std::list to prevent invalidating any
			// pointers, however, since all blocks will be registered in ONE go
			// from a Lua initialisation, these pointers will not be invalidated
			// for their whole lifetime, until the block registry is destroyed -
			// but that will be quite late in the destruction of the program so
			// this *shouldn't* be an issue. - @beeperdeeper089
			std::vector<BlockType> m_blocks;
			TextureRegistry        m_textures;
		};
	} // namespace voxels
} // namespace q2
