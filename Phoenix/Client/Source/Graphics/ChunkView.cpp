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

#include <Client/Graphics/ChunkView.hpp>
#include <Client/Graphics/ChunkMesher.hpp>

#include <Common/PlayerView.hpp>
#include <Common/Voxels/BlockRegistry.hpp>

using namespace phx::voxels;
using namespace phx;

ChunkView::ChunkView(int viewDistance, entt::registry* registry,
                     entt::entity entity)
    : m_viewDistance(viewDistance), m_entity(entity), m_registry(registry)
{
	// calculates the maximum visible chunks.
	const int viewLength       = (viewDistance * 2) + 1;
	const int maxVisibleChunks = viewLength * viewLength * viewLength;

	m_renderer = new gfx::ChunkRenderer(maxVisibleChunks);
	m_renderer->buildTextureArray();
}

ChunkView::~ChunkView()
{
	delete m_renderer;
}

void ChunkView::tick()
{
	for (auto& chunk : PlayerView::update(m_registry, m_entity))
	{
		gfx::ChunkMesher mesher(chunk->getChunkPos(), chunk->getBlocks(),
		                        m_renderer->getTextureTable());
		mesher.mesh();
		m_renderer->submitChunk(mesher.getMesh(), chunk->getChunkPos());
	}
}

void ChunkView::render() { m_renderer->render(); }

BlockType* ChunkView::getBlockAt(math::vec3 position) const
{
	return m_registry->get<PlayerView>(m_entity).map->getBlockAt(position);
}

void ChunkView::setBlockAt(math::vec3 position, BlockType* block)
{
	auto map = m_registry->get<PlayerView>(m_entity).map;
	map->setBlockAt(position, block);

	Chunk*           chunk = map->getChunk(Map::getBlockPos(position).first);
	gfx::ChunkMesher mesher(chunk->getChunkPos(), chunk->getBlocks(),
	                        m_renderer->getTextureTable());
	mesher.mesh();
	m_renderer->updateChunk(mesher.getMesh(), chunk->getChunkPos());
}
