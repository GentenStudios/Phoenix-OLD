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

/**
 * @file ChunkRenderer.hpp
 * @brief The ChunkRenderer.
 * 
 * @copyright Copyright (c) 2019-20 Genten Studios
 */

#pragma once

#include <Phoenix/Graphics/ShaderPipeline.hpp>

#include <unordered_map>
#include <vector>

namespace phx
{
	namespace gfx
	{
		// forward declaration
		class ShaderPipeline;

		/**
		 * @brief A struct to store the data required to render chunks.
		 *
		 * Don't worry about this if you don't have OpenGL knowledge, this is
		 * mainly an internal data format anyway.
		 */
		struct ChunkRenderData
		{
			/// @brief The vertex array object.
			unsigned int vao;
			/// @brief The buffer object.
			unsigned int buffer;
			/// @brief The amount of vertices to render.
			std::size_t vertexCount;
		};

		/**
		 * @brief Renders submitted chunks, and allows for dropping and updating of chunks.
		 *
		 * This chunk renderer must be constructed with the number of max view distance.
		 *
		 * Before this is initialized, try to make sure all blocks are loaded in
		 * through the Lua subsystem. This can ensure that we can build the
		 * texture array and have every texture already in there and not have to
		 * deal with a broken texture array where some textures are missing.
		 * Currently, we have a limitation of 256 textures that can be loaded at
		 * any one time, since we're using OpenGL 3.3 which only guarantees 256.
		 *
		 * @paragraph Usage
		 * @code
		 * ChunkRenderer* renderer = new ChunkRenderer(2); // view distance of 3.
		 * renderer->buildTextureArray(); // make sure all blocks are loaded in before this.
		 *
		 * Chunk chunk = Chunk({0, 0, 0});
		 *
 		 * ChunkMesher mesher({0, 0, 0}, chunk, renderer->getTextureTable());
		 * mesher.mesh();
		 *
		 * renderer->submitChunk(mesher.getMesh(), { 0, 0, 0 });
		 * //renderer->updateChunk(mesher.getMesh(), { 0, 0, 0 });
		 * //renderer->dropChunk({ 0, 0, 0 });
		 *
		 * renderer->render():
		 * @endcode
		 *
		 * @todo Find solution to the max texture limit. Not urgent but hopefully by 0.3/4
		 */
		class ChunkRenderer
		{
		public:
			/// @brief Typedef to make it easier for use outside of this class.
			using AssociativeTextureTable =
			    std::unordered_map<std::string, std::size_t>;

			/**
			 * @brief Constructs a chunk renderer which will accept a specific amount of chunks.
			 * @param visibleChunks The maximum view distance.
			 *
			 * The max view distance does not do anything yet, mainly because of
			 * a redesign of renderer which ended up not requiring that, however
			 * it will become used as I (beeperdeeper089) continue to improve
			 * this functionality.
			 */
			ChunkRenderer(std::size_t visibleChunks);
			~ChunkRenderer();

			/**
			 * @brief Gets the shader vertex layout that this renderer requires.
			 * @return The layout that the ShaderPipeline needs to guarantee.
			 */
			static std::vector<ShaderLayout> getRequiredShaderLayout();

			/**
			 * @brief Builds a texture array for everything to use.
			 *
			 * This builds a texture array using all the textures registered in
			 * the TextureRegistry, which comes from the BlockRegistry in this
			 * context.
			 *
			 * This does have the caveat of being unable to be larger than 256
			 * layers, aka 256 textures - a texture array is essentially a 3D
			 * array where the Z-axis can just be layer that we are storing the
			 * texture on. This ia a OpenGL thing so internal understanding is
			 * not necessary unless modifying/improving this function.
			 */
			void                           buildTextureArray();
			
			/**
			 * @brief Gets the table telling eveyrthing where each texture is in the GPU-side array.
			 * @return An associative table storing which textures are on which layers within the texture array.
			 */
			const AssociativeTextureTable& getTextureTable() const;

			/**
			 * @brief Uploads and prepares a chunk's Mesh for rendering.
			 * @param mesh The meshed data to upload and render.
			 * @param pos The position of the chunk that the mesh of for.
			 *
			 * This function does not check if a mesh for requested chunk exists
			 * beforehand so keep track manually and make sure you're not
			 * attempting to submit the same chunk multiple times. This is not
			 * the fastest function, so if the chunk already exists, don't just
			 * drop and submit again, try to update since that will be more
			 * efficient in the long run.
			 *
			 * If the chunk has no vertices (all air), it will not create an
			 * entry in the internal list, since there is no point, however, a
			 * redundancy has been built into updateChunk just in case, to make
			 * sure you don't have to check whether the chunk is air manually
			 * and knowing whether it is submitted or not.
			 */
			void submitChunk(const std::vector<float>& mesh, math::vec3 pos);

			/**
			 * @brief Updates the new mesh for a previously submitted chunk.
			 * @param mesh The mesh of the updated chunk.
			 * @param pos The position of the chunk you're updating.
			 *
			 * This function has a redundancy that will automatically run
			 * "submitChunk" if the chunk doesn't exist in the internal list.
			 * This is more efficient for updating chunks since it won't
			 * reallocate the GPU-side buffer if the mesh size is the same.
			 */
			void updateChunk(const std::vector<float>& mesh, math::vec3 pos);

			/**
			 * @brief Deletes the stated chunk from the GPU.
			 * @param pos The position of the chunk to drop.
			 */
			void dropChunk(math::vec3 pos);

			/**
			 * @brief Renders the active chunks.
			 */
			void render();

		private:
			std::size_t m_visibleChunks;

			std::unordered_map<math::vec3, ChunkRenderData, math::Vector3Hasher,
			                   math::Vector3KeyComparator>
			             m_buffers;
			unsigned int m_textureArray;

			const int m_vertexAttributeLocation = 0;
			const int m_uvAttributeLocation     = 1;

			AssociativeTextureTable m_textureTable;
		};
	} // namespace gfx
} // namespace phx
