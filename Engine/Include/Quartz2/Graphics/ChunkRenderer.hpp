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

#include <Quartz2/Graphics/ShaderPipeline.hpp>

#include <unordered_map>
#include <vector>

namespace q2
{
	namespace gfx
	{
		class ShaderPipeline;

		// NOTE: (mainly to self - @beeperdeeper089)
		// All blocks must be loaded in before the texture array is generated.
		// Once the texture array is generated, no new blocks should be loaded
		// AT ALL. This will require a rebuild of the array and I'm not going to
		// build that functionality in until we need it, since we're using
		// OpenGL 3.3 which only supports a *minimum of 256*, giving us not that
		// much space to work with as of right now. Currently we will impose a
		// hard limit of 256 textures, and once we overcome that issue, we can
		// revise this documentation.
		class ChunkRenderer
		{
		public:
			using MeshIdentifier = int;
			using AssociativeTextureTable =
			    std::unordered_map<std::string, std::size_t>;

			ChunkRenderer(std::size_t visibleChunks);
			~ChunkRenderer();

			std::vector<ShaderLayout> getRequiredShaderLayout();

			void                           buildTextureArray();
			const AssociativeTextureTable& getTextureTable() const;

			// returns unique chunk mesh id. Used to set the render list.
			MeshIdentifier submitChunkMesh(const std::vector<float>& mesh,
			                               MeshIdentifier            slot);

			void render();

		private:
			std::size_t m_visibleChunks;

			unsigned int m_vao;
			unsigned int m_buffer;
			unsigned int m_textureArray;

			const int m_vertexAttributeLocation   = 0;
			const int m_uvAttributeLocation       = 1;
			const int m_texLayerAttributeLocation = 2;

			AssociativeTextureTable m_textureTable;

			std::vector<int>     m_multiDrawStarts;
			std::vector<GLsizei> m_multiDrawCounts;
			std::vector<int>     m_bigBufferLocations;
		};
	} // namespace gfx
} // namespace q2