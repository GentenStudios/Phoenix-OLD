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
#include <Client/Voxels/BlockRegistry.hpp>

#include <Common/Voxels/Chunk.hpp>

#include <vector>

namespace phx::gfx
{
	/**
	 * @brief Meshes a chunk.
	 *
	 * This mesher does not understand "smart"/"greedy" meshing, it will mesh
	 * only this chunk, and will not take neighbor chunks into account as of
	 * yet. As the project gains maturity and we have more features, this will
	 * be improved.
	 *
	 * @paragraph Usage
	 * @code
	 * auto mesh = ChunkMesher::mesh(chunk, renderer->getTextureTable(),
	 * blockRegistry);
	 * @endcode
	 *
	 */
	class ChunkMesher
	{
	public:
		static std::vector<float> mesh(
		    voxels::Chunk*                                chunk,
		    const ChunkRenderer::AssociativeTextureTable& texTable,
		    client::BlockRegistry*                        blockRegistry);
	};
} // namespace phx::gfx
