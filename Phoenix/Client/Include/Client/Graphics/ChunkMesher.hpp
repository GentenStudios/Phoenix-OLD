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
 * @file ChunkMesher.hpp
 * @brief The mesher class for use with chunks.
 *
 * @copyright Copyright (c) 2019-20 Genten Studios
 */

#pragma once

#include <Client/Graphics/ChunkRenderer.hpp>

#include <Common/Math/Math.hpp>
#include <Common/Voxels/Block.hpp>

#include <vector>
#include <glm/glm.hpp>

namespace phx::gfx
{
	/**
	 * @brief Each face of a cube.
	 *
	 * This is the order in which textures in Lua MUST be registered. The
	 * system only understands this format, or textures will be the wrong
	 * way round and you won't know why.
	 */
	enum class BlockFace : unsigned int
	{
		FRONT = 0,
		LEFT,
		BACK,
		RIGHT,
		TOP,
		BOTTOM
	};

	/**
	 * @brief Meshes a chunk.
	 *
	 * Once provided with a reference to the blocks within a chunk and a
	 * ready built texture table gotten from the ChunkRenderer, it can mesh
	 * the chunks very simply.
	 *
	 * This mesher does not understand "smart" meshing, it will mesh only
	 * this chunk, and will not take neighbor chunks into account as of yet.
	 * As the project gains maturity and we have more features, this will be
	 * improved.
	 *
	 * @paragraph Usage
	 * @code
	 * ChunkMesher mesher(chunkPosition, chunk.getBlocks(),
	 * renderer->getTextureTable()) mesher.mesh()
	 * renderer->submitChunk(mesher.getMesh(), chunkPosition);
	 * @endcode
	 *
	 */
	class ChunkMesher
	{
	public:
		/**
		 * @brief Constructs the mesher based on a few parameters.
		 * @param pos The position of the chunk.
		 * @param blocks A reference to the array of blocks in the chunk.
		 * @param texTable The texture table values created by the renderer.
		 */
		ChunkMesher(math::vec3 pos, std::vector<voxels::BlockType*>& blocks,
		            const ChunkRenderer::AssociativeTextureTable& texTable)
		    : m_pos(pos), m_blockRef(blocks), m_texTable(texTable)
		{
		}
		~ChunkMesher() = default;

		/**
		 * @brief Meshes the chunk.
		 */
		void mesh();

		/**
		 * @brief Returns the mesh as an array of floats.
		 * @return The mesh as an array of floats.
		 */
		const std::vector<float>& getMesh() const { return m_mesh; }
		
	private:
		// internal function, no need to document.
		void addBlockFace(voxels::BlockType* block, BlockFace face, float x,
		                  float y, float z,  glm::vec3 normals);

	private:
		math::vec3                                    m_pos;
		std::vector<float>                            m_mesh;
		std::vector<voxels::BlockType*>               m_blockRef;
		const ChunkRenderer::AssociativeTextureTable& m_texTable;
	};
} // namespace phx::gfx

