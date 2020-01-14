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

#include <Phoenix/Graphics/ChunkMesher.hpp>
#include <Phoenix/Voxels/Chunk.hpp>

#include <iostream>

static const q2::math::vec3 CUBE_VERTS[] = {
    // front
    q2::math::vec3(-1.f, -1.f, -1.f), q2::math::vec3(1.f, -1.f, -1.f),
    q2::math::vec3(1.f, 1.f, -1.f), q2::math::vec3(1.f, 1.f, -1.f),
    q2::math::vec3(-1.f, 1.f, -1.f), q2::math::vec3(-1.f, -1.f, -1.f),

	// left
    q2::math::vec3(-1.f, 1.f, 1.f), q2::math::vec3(-1.f, 1.f, -1.f),
    q2::math::vec3(-1.f, -1.f, -1.f), q2::math::vec3(-1.f, -1.f, -1.f),
    q2::math::vec3(-1.f, -1.f, 1.f), q2::math::vec3(-1.f, 1.f, 1.f),

    // back
    q2::math::vec3(-1.f, -1.f, 1.f), q2::math::vec3(1.f, -1.f, 1.f),
    q2::math::vec3(1.f, 1.f, 1.f), q2::math::vec3(1.f, 1.f, 1.f),
    q2::math::vec3(-1.f, 1.f, 1.f), q2::math::vec3(-1.f, -1.f, 1.f),

    // right
    q2::math::vec3(1.f, 1.f, 1.f), q2::math::vec3(1.f, 1.f, -1.f),
    q2::math::vec3(1.f, -1.f, -1.f), q2::math::vec3(1.f, -1.f, -1.f),
    q2::math::vec3(1.f, -1.f, 1.f), q2::math::vec3(1.f, 1.f, 1.f),

	// top
    q2::math::vec3(-1.f, 1.f, -1.f), q2::math::vec3(1.f, 1.f, -1.f),
    q2::math::vec3(1.f, 1.f, 1.f), q2::math::vec3(1.f, 1.f, 1.f),
    q2::math::vec3(-1.f, 1.f, 1.f), q2::math::vec3(-1.f, 1.f, -1.f),

    // bottom
    q2::math::vec3(-1.f, -1.f, -1.f), q2::math::vec3(1.f, -1.f, -1.f),
    q2::math::vec3(1.f, -1.f, 1.f), q2::math::vec3(1.f, -1.f, 1.f),
    q2::math::vec3(-1.f, -1.f, 1.f), q2::math::vec3(-1.f, -1.f, -1.f),
};

static const q2::math::vec2 CUBE_UV[] = {
    // front north
    q2::math::vec2(-0.f, 1.f),
    q2::math::vec2(-1.f, 1.f),
    q2::math::vec2(-1.f, 0.f),
    q2::math::vec2(-1.f, 0.f),
    q2::math::vec2(-0.f, 0.f),
    q2::math::vec2(-0.f, 1.f),

	// left west
    q2::math::vec2(-0.f, 0.f),
    q2::math::vec2(-1.f, 0.f),
    q2::math::vec2(-1.f, 1.f),
    q2::math::vec2(-1.f, 1.f),
    q2::math::vec2(-0.f, 1.f),
    q2::math::vec2(-0.f, 0.f),

    // back south
    q2::math::vec2(0.f, 1.f),
    q2::math::vec2(1.f, 1.f),
    q2::math::vec2(1.f, 0.f),
    q2::math::vec2(1.f, 0.f),
    q2::math::vec2(0.f, 0.f),
    q2::math::vec2(0.f, 1.f),

	// right east
    q2::math::vec2(0.f, 0.f),
    q2::math::vec2(1.f, 0.f),
    q2::math::vec2(1.f, 1.f),
    q2::math::vec2(1.f, 1.f),
    q2::math::vec2(0.f, 1.f),
    q2::math::vec2(0.f, 0.f),

    // top
    q2::math::vec2(0.f, -1.f),
    q2::math::vec2(1.f, -1.f),
    q2::math::vec2(1.f, -0.f),
    q2::math::vec2(1.f, -0.f),
    q2::math::vec2(0.f, -0.f),
    q2::math::vec2(0.f, -1.f),

    // bottom
    q2::math::vec2(0.f, 1.f),
    q2::math::vec2(1.f, 1.f),
    q2::math::vec2(1.f, 0.f),
    q2::math::vec2(1.f, 0.f),
    q2::math::vec2(0.f, 0.f),
    q2::math::vec2(0.f, 1.f),
};

const int ACTUAL_CUBE_SIZE  = 2;
const int NUM_FACES_IN_CUBE = 6;
const int NUM_VERTS_IN_FACE = 6;

using namespace q2;
using namespace gfx;

void ChunkMesher::mesh()
{
	using namespace voxels;
	for (std::size_t i = 0;
	     i < Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH; ++i)
	{
		BlockType* block = m_blockRef[i];

		if (block->category != BlockCategory::SOLID)
			continue;

		const std::size_t x = i % Chunk::CHUNK_WIDTH;
		const std::size_t y = (i / Chunk::CHUNK_WIDTH) % Chunk::CHUNK_HEIGHT;
		const std::size_t z = i / (Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT);

		if (x == 0 ||
		    m_blockRef[Chunk::getVectorIndex(x - 1, y, z)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::LEFT, static_cast<float>(x),
			             static_cast<float>(y), static_cast<float>(z));
		if (x == Chunk::CHUNK_WIDTH - 1 ||
		    m_blockRef[Chunk::getVectorIndex(x + 1, y, z)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::RIGHT, static_cast<float>(x),
			             static_cast<float>(y), static_cast<float>(z));

		if (y == 0 ||
		    m_blockRef[Chunk::getVectorIndex(x, y - 1, z)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::BOTTOM, static_cast<float>(x),
			             static_cast<float>(y), static_cast<float>(z));
		if (y == Chunk::CHUNK_WIDTH - 1 ||
		    m_blockRef[Chunk::getVectorIndex(x, y + 1, z)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::TOP, static_cast<float>(x),
			             static_cast<float>(y), static_cast<float>(z));

		if (z == 0 ||
		    m_blockRef[Chunk::getVectorIndex(x, y, z - 1)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::FRONT, static_cast<float>(x),
			             static_cast<float>(y), static_cast<float>(z));
		if (z == Chunk::CHUNK_DEPTH - 1 ||
		    m_blockRef[Chunk::getVectorIndex(x, y, z + 1)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::BACK, static_cast<float>(x),
			             static_cast<float>(y), static_cast<float>(z));
	}
}

void ChunkMesher::addBlockFace(voxels::BlockType* block, BlockFace face,
                               float x, float y, float z)
{
	int texLayer = -1;

	texLayer = m_texTable.at(block->textures[static_cast<std::size_t>(face)]);

	for (int i = 0; i < NUM_VERTS_IN_FACE; ++i)
	{
		math::vec3 blockVertices = CUBE_VERTS[(static_cast<int>(face) * NUM_FACES_IN_CUBE) + i];
		blockVertices.x += (x * ACTUAL_CUBE_SIZE) + (m_pos.x * ACTUAL_CUBE_SIZE);
		blockVertices.y += (y * ACTUAL_CUBE_SIZE) + (m_pos.y * ACTUAL_CUBE_SIZE);
		blockVertices.z += (z * ACTUAL_CUBE_SIZE) + (m_pos.z * ACTUAL_CUBE_SIZE);

		math::vec2 cubeUVs = CUBE_UV[(static_cast<int>(face) * NUM_FACES_IN_CUBE) + i];

		m_mesh.push_back(blockVertices.x);
		m_mesh.push_back(blockVertices.y);
		m_mesh.push_back(blockVertices.z);

		m_mesh.push_back(cubeUVs.x);
		m_mesh.push_back(cubeUVs.y);

		m_mesh.push_back(texLayer);
	}
}
