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
 * @file ChunkRenderer.hpp
 * @brief The ChunkRenderer.
 *
 * @copyright Copyright (c) 2019-20 Genten Studios
 */

#pragma once

#include <Client/Graphics/Camera.hpp>
#include <Client/Graphics/ShaderPipeline.hpp>
#include <Client/Graphics/TexturePacker.hpp>
#include <Client/Voxels/BlockRegistry.hpp>

#include <Common/Voxels/Map.hpp>

#include <entt/entt.hpp>

#include <Common/Position.hpp>
#include <unordered_map>
#include <vector>

namespace phx::gfx
{
	struct Vertex
	{
		math::vec3 pos;
		math::vec3 uv;
		math::vec3 normal;
	};

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
        unsigned int vbo;
        /// @brief The amount of vertices to render.
        std::size_t vertexCount;
    };

	/**
	 * @brief Renders submitted chunks, and allows for dropping and updating of
	 * chunks.
	 *
	 * This chunk renderer must be constructed with the number of max view
	 * distance.
	 *
	 * Before this is initialized, try to make sure all blocks are loaded in
	 * through the Lua subsystem. This can ensure that we can build the
	 * texture array and have every texture already in there and not have to
	 * deal with a broken texture array where some textures are missing.
	 *
	 * @paragraph Usage
	 * @code
	 * ChunkRenderer* renderer = new ChunkRenderer(map, blockRegistry,
	 *                             entityRegistry, entity);
	 * renderer->prep();
	 * renderer->attachCamera();
	 *
	 * Chunk chunk = Chunk({0, 0, 0});
	 *
	 * renderer->add(chunk);
	 * //renderer->update(chunk);
	 * //renderer->remove(chunk);
	 *
	 * renderer->tick():
	 * @endcode
	 */
	class ChunkRenderer : public voxels::MapEventSubscriber
	{
	public:

		/// @brief Typedef to make it easier for use outside of this class.
		using AssociativeTextureTable =
		    std::unordered_map<std::string, std::size_t>;

		ChunkRenderer(client::BlockRegistry* blockRegistry);
		~ChunkRenderer();

		void prep();

    private:
        static ChunkRenderData generate(std::vector<float> mesh);

	public:
		void add(voxels::Chunk* chunk);
		void update(voxels::Chunk* chunk);
		void remove(voxels::Chunk* chunk);
		void clear();

		void onMapEvent(const voxels::MapEvent& mapEvent) override;

		void tick(entt::registry* registry, entt::entity entity, const math::mat4& projection, const float& dt);

		/**
		 * @brief Gets the shader vertex layout that this renderer requires.
		 * @return The layout that the ShaderPipeline needs to guarantee.
		 */
		static std::vector<ShaderLayout> getRequiredShaderLayout();

		/**
		 * @brief Gets the table telling everything where each texture is in the
		 * GPU-side array.
		 * @return An associative table storing which textures are on which
		 * layers within the texture array.
		 */
		const AssociativeTextureTable& getTextureTable() const;

	private:
		client::BlockRegistry* m_blockRegistry;
		BlockingQueue<voxels::MapEvent> m_mapEvents;

		// keep another copy of chunk pointers so we can know which chunks need
		// to be remeshed that are being rendered rn.
		std::vector<voxels::Chunk*> m_chunks;

		std::unordered_map<math::vec3, ChunkRenderData, math::Vector3Hasher,
		                   math::Vector3KeyComparator>
		    m_buffers;

		static const int m_vertexAttributeLocation = 0;
		static const int m_uvAttributeLocation     = 1;
		static const int m_normalAttributeLocation = 2;
		static const int m_colorAttributeLocation  = 3;

        gfx::ShaderPipeline m_renderPipeline = {};
	};
} // namespace phx::gfx
