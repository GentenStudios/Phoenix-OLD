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

#include <Quartz2/Math/Math.hpp>
#include <Quartz2/Voxels/Block.hpp>
#include <Quartz2/Graphics/ChunkRenderer.hpp>

#include <vector>

namespace q2
{
	namespace gfx
	{
		enum class BlockFace : unsigned int
		{
			FRONT = 0,
			LEFT,
			BACK,
			RIGHT,
			TOP,
			BOTTOM
		};

		class ChunkMesher
		{
		public:
			ChunkMesher(math::vec3 pos, std::vector<voxels::BlockType*>& blocks, const ChunkRenderer::AssociativeTextureTable& texTable)
			    : m_blockRef(blocks), m_pos(pos), m_texTable(texTable)
			{
			}
			~ChunkMesher() = default;

			void mesh();
			const std::vector<float>& getMesh() { return m_mesh; }

		private:
			void addBlockFace(voxels::BlockType* block, BlockFace face, float x, float y, float z);

		private:
			math::vec3                      m_pos;
			std::vector<float>              m_mesh;
			std::vector<voxels::BlockType*> m_blockRef;
			const ChunkRenderer::AssociativeTextureTable& m_texTable;
		};
	} // namespace gfx
} // namespace q2