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

#include <Client/Graphics/ChunkMesher.hpp>
#include <Common/Voxels/Chunk.hpp>

static const phx::math::vec3 CUBE_VERTS[] = {
    // front
    phx::math::vec3(-1.f, -1.f, -1.f),
    phx::math::vec3(1.f, -1.f, -1.f),
    phx::math::vec3(1.f, 1.f, -1.f),
    phx::math::vec3(1.f, 1.f, -1.f),
    phx::math::vec3(-1.f, 1.f, -1.f),
    phx::math::vec3(-1.f, -1.f, -1.f),

    // left
    phx::math::vec3(-1.f, 1.f, 1.f),
    phx::math::vec3(-1.f, 1.f, -1.f),
    phx::math::vec3(-1.f, -1.f, -1.f),
    phx::math::vec3(-1.f, -1.f, -1.f),
    phx::math::vec3(-1.f, -1.f, 1.f),
    phx::math::vec3(-1.f, 1.f, 1.f),

    // back
    phx::math::vec3(-1.f, -1.f, 1.f),
    phx::math::vec3(1.f, -1.f, 1.f),
    phx::math::vec3(1.f, 1.f, 1.f),
    phx::math::vec3(1.f, 1.f, 1.f),
    phx::math::vec3(-1.f, 1.f, 1.f),
    phx::math::vec3(-1.f, -1.f, 1.f),

    // right
    phx::math::vec3(1.f, 1.f, 1.f),
    phx::math::vec3(1.f, 1.f, -1.f),
    phx::math::vec3(1.f, -1.f, -1.f),
    phx::math::vec3(1.f, -1.f, -1.f),
    phx::math::vec3(1.f, -1.f, 1.f),
    phx::math::vec3(1.f, 1.f, 1.f),

    // top
    phx::math::vec3(-1.f, 1.f, -1.f),
    phx::math::vec3(1.f, 1.f, -1.f),
    phx::math::vec3(1.f, 1.f, 1.f),
    phx::math::vec3(1.f, 1.f, 1.f),
    phx::math::vec3(-1.f, 1.f, 1.f),
    phx::math::vec3(-1.f, 1.f, -1.f),

    // bottom
    phx::math::vec3(-1.f, -1.f, -1.f),
    phx::math::vec3(1.f, -1.f, -1.f),
    phx::math::vec3(1.f, -1.f, 1.f),
    phx::math::vec3(1.f, -1.f, 1.f),
    phx::math::vec3(-1.f, -1.f, 1.f),
    phx::math::vec3(-1.f, -1.f, -1.f),
};

static const phx::math::vec2 CUBE_UV[] = {
    // front north
    phx::math::vec2(-0.f, 1.f),
    phx::math::vec2(-1.f, 1.f),
    phx::math::vec2(-1.f, 0.f),
    phx::math::vec2(-1.f, 0.f),
    phx::math::vec2(-0.f, 0.f),
    phx::math::vec2(-0.f, 1.f),

    // left west
    phx::math::vec2(-0.f, 0.f),
    phx::math::vec2(-1.f, 0.f),
    phx::math::vec2(-1.f, 1.f),
    phx::math::vec2(-1.f, 1.f),
    phx::math::vec2(-0.f, 1.f),
    phx::math::vec2(-0.f, 0.f),

    // back south
    phx::math::vec2(0.f, 1.f),
    phx::math::vec2(1.f, 1.f),
    phx::math::vec2(1.f, 0.f),
    phx::math::vec2(1.f, 0.f),
    phx::math::vec2(0.f, 0.f),
    phx::math::vec2(0.f, 1.f),

    // right east
    phx::math::vec2(0.f, 0.f),
    phx::math::vec2(1.f, 0.f),
    phx::math::vec2(1.f, 1.f),
    phx::math::vec2(1.f, 1.f),
    phx::math::vec2(0.f, 1.f),
    phx::math::vec2(0.f, 0.f),

    // top
    phx::math::vec2(0.f, -1.f),
    phx::math::vec2(1.f, -1.f),
    phx::math::vec2(1.f, -0.f),
    phx::math::vec2(1.f, -0.f),
    phx::math::vec2(0.f, -0.f),
    phx::math::vec2(0.f, -1.f),

    // bottom
    phx::math::vec2(0.f, 1.f),
    phx::math::vec2(1.f, 1.f),
    phx::math::vec2(1.f, 0.f),
    phx::math::vec2(1.f, 0.f),
    phx::math::vec2(0.f, 0.f),
    phx::math::vec2(0.f, 1.f),
};

const int ACTUAL_CUBE_SIZE  = 2;
const int NUM_FACES_IN_CUBE = 6;
const int NUM_VERTS_IN_FACE = 6;

using namespace phx;
using namespace gfx;

std::vector<float> ChunkMesher::mesh(
    voxels::Chunk*                                chunk,
    const ChunkRenderer::AssociativeTextureTable& texTable,
    client::BlockRegistry*                        blockRegistry)
{
	std::vector<float> mesh;

	auto&      blocks   = chunk->getBlocks();
	math::vec3 chunkPos = chunk->getChunkPos();

	// funky lambda here, fix in the future, but this is actually a bit tidier
	// than otherwise.
	auto addBlockFace = [&mesh, &chunkPos, &blockRegistry,
	                     &texTable](voxels::BlockType* block, BlockFace face,
	                                math::vec3 pos) {
		// we don't need to worry about nullptr being returned here since we
		// have
		// setup the unknown return val in the BlockRegistry constructor.
		const auto* textures =
		    blockRegistry->textures.get(block->uniqueIdentifier);

		std::size_t texLayer = 0;
		if (textures->size() != 6)
		{
			// we can ALWAYS guarantee 1 texture since core:unknown will have
			// unknown.png registered. (the textures for core:unknown will be
			// returned from the texture registry if a block's tex are not
			// found.)
			texLayer = texTable.at((*textures)[0]);
		}
		else
		{
			texLayer = texTable.at((*textures)[static_cast<std::size_t>(face)]);
		}

		math::vec3 normals;
		switch (face)
		{
		case BlockFace::FRONT:
			normals = math::vec3(0, 0, -1);
			break;

		case BlockFace::LEFT:
			normals = math::vec3(-1, 0, 0);
			break;

		case BlockFace::BACK:
			normals = math::vec3(0, 0, 1);
			break;

		case BlockFace::RIGHT:
			normals = math::vec3(1, 0, 0);
			break;

		case BlockFace::TOP:
			normals = math::vec3(0, 1, 0);
			break;

		case BlockFace::BOTTOM:
			normals = math::vec3(0, -1, 0);
			break;
		}

		for (int i = 0; i < NUM_VERTS_IN_FACE; ++i)
		{
			math::vec3 blockVertices =
			    CUBE_VERTS[(static_cast<int>(face) * NUM_FACES_IN_CUBE) + i];
			blockVertices.x +=
			    (pos.x * ACTUAL_CUBE_SIZE) + (chunkPos.x * ACTUAL_CUBE_SIZE);
			blockVertices.y +=
			    (pos.y * ACTUAL_CUBE_SIZE) + (chunkPos.y * ACTUAL_CUBE_SIZE);
			blockVertices.z +=
			    (pos.z * ACTUAL_CUBE_SIZE) + (chunkPos.z * ACTUAL_CUBE_SIZE);

			math::vec2 cubeUVs =
			    CUBE_UV[(static_cast<int>(face) * NUM_FACES_IN_CUBE) + i];

			mesh.push_back(blockVertices.x);
			mesh.push_back(blockVertices.y);
			mesh.push_back(blockVertices.z);

			mesh.push_back(cubeUVs.x);
			mesh.push_back(cubeUVs.y);

			mesh.push_back(static_cast<float>(texLayer));

			mesh.push_back(normals.x);
			mesh.push_back(normals.y);
			mesh.push_back(normals.z);

			mesh.push_back(block->color);
		}
	};

	using namespace voxels;
	for (std::size_t i = 0;
	     i < Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH; ++i)
	{
		BlockType* block = blocks[i];

		if (block->category != BlockCategory::SOLID)
			continue;

		const std::size_t x = i % Chunk::CHUNK_WIDTH;
		const std::size_t y = (i / Chunk::CHUNK_WIDTH) % Chunk::CHUNK_HEIGHT;
		const std::size_t z = i / (Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT);

		if (x == 0 || blocks[Chunk::getVectorIndex(x - 1, y, z)]->category !=
		                  BlockCategory::SOLID)
			addBlockFace(block, BlockFace::LEFT, {x, y, z});
		if (x == Chunk::CHUNK_WIDTH - 1 ||
		    blocks[Chunk::getVectorIndex(x + 1, y, z)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::RIGHT, {x, y, z});

		if (y == 0 || blocks[Chunk::getVectorIndex(x, y - 1, z)]->category !=
		                  BlockCategory::SOLID)
			addBlockFace(block, BlockFace::BOTTOM, {x, y, z});
		if (y == Chunk::CHUNK_WIDTH - 1 ||
		    blocks[Chunk::getVectorIndex(x, y + 1, z)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::TOP, {x, y, z});

		if (z == 0 || blocks[Chunk::getVectorIndex(x, y, z - 1)]->category !=
		                  BlockCategory::SOLID)
			addBlockFace(block, BlockFace::FRONT, {x, y, z});
		if (z == Chunk::CHUNK_DEPTH - 1 ||
		    blocks[Chunk::getVectorIndex(x, y, z + 1)]->category !=
		        BlockCategory::SOLID)
			addBlockFace(block, BlockFace::BACK, {x, y, z});
	}

	return mesh;
}
