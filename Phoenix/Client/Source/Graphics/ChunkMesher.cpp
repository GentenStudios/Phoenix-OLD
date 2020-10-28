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

#include <Client/Graphics/BaseBlockModels.hpp>
#include <Client/Graphics/ChunkMesher.hpp>

using namespace phx::gfx;

std::vector<float> ChunkMesher::mesh(
    phx::voxels::Chunk*                           chunk,
    const ChunkRenderer::AssociativeTextureTable& texTable,
    phx::client::BlockRegistry*                   blockRegistry)
{
	std::vector<float> mesh;

	auto&           blocks   = chunk->getBlocks();
	phx::math::vec3 chunkPos = chunk->getChunkPos();

	using namespace voxels;
	for (std::size_t i = 0;
	     i < Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH; ++i)
	{
		BlockType* block = blocks[i];

		if (block->category != BlockCategory::SOLID)
			continue;

		// get position of block in chunk.
		const std::size_t x = i % Chunk::CHUNK_WIDTH;
		const std::size_t y = (i / Chunk::CHUNK_WIDTH) % Chunk::CHUNK_HEIGHT;
		const std::size_t z = i / (Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT);

		// get textures since at this point we know we're gonna be meshing something.
		const auto* tex = blockRegistry->textures.get(block->uniqueIdentifier);

		auto insertToMesh = [&mesh, tex, texTable, chunkPos](DefaultMeshVertex const* vertex, std::size_t vertexCount, BlockFace face, const math::vec3& blockPos)
		{
			std::size_t texLayer = 0;
			if ((*tex).size() != 6)
			{
				texLayer = texTable.at((*tex)[0]);
			}
			else
			{
				texLayer = texTable.at((*tex)[static_cast<int>(face)]);
			}
			
			for (std::size_t i = 0; i < vertexCount; ++i)
			{
				DefaultMeshVertex const* current = vertex + i;
			
				mesh.push_back(current->pos.x + (blockPos.x * DEFAULT_MODEL_SIZE) + (chunkPos.x * DEFAULT_MODEL_SIZE));
				mesh.push_back(current->pos.y + (blockPos.y * DEFAULT_MODEL_SIZE) + (chunkPos.y * DEFAULT_MODEL_SIZE));
				mesh.push_back(current->pos.z + (blockPos.z * DEFAULT_MODEL_SIZE) + (chunkPos.z * DEFAULT_MODEL_SIZE));

				mesh.push_back(current->uv.x);
				mesh.push_back(current->uv.y);
				mesh.push_back(texLayer);

				mesh.push_back(current->normal.x);
				mesh.push_back(current->normal.y);
				mesh.push_back(current->normal.z);
			}
		};

		// Meshing design thoughts:
		//		We want meshing to be efficient, speedy but not too lenient on the amount of vertices produced.
		//		To support this, I (beeper) think it's probably beneficial to cull faces of normal blocks next to each other, and just let other block models just add all their vertices.
		//		We can explore this further at a later point in time, since it's just not that much of a concern right now.
		//		If performance really tanks with the introduction of all the extra vertices, we will have to take a look sooner than later, but a couple hundred extra verts shouldn't kill any modern GPU.
		//		This was written 1st Oct, 2020.

		BlockModel blockModel =
		    *blockRegistry->models.get(block->uniqueIdentifier);
		
		switch (blockModel)
		{
		case BlockModel::BLOCK:
			{
				// check if block is on the north border of the chunk.
				if (z == 0)
				{
					// if on north border, add north face.
					insertToMesh(BLOCK_FRONT, BLOCK_FACE_VERT_COUNT, BlockFace::NORTH, {x, y, z});
				}
				else
				{
					// else get the block to the north.
					BlockType* north = blocks[Chunk::getVectorIndex(x, y, z - 1)];

					// if the block to the north is not solid, or is not a full block, add the north face.
					if (north->category != BlockCategory::SOLID || *blockRegistry->models.get(north->uniqueIdentifier) != BlockModel::BLOCK)
					{
						insertToMesh(BLOCK_FRONT, BLOCK_FACE_VERT_COUNT, BlockFace::NORTH, {x, y, z});
					}
				}

				// check if the block is on the south border
				if (z == Chunk::CHUNK_DEPTH - 1)
				{
					insertToMesh(BLOCK_BACK, BLOCK_FACE_VERT_COUNT, BlockFace::SOUTH, {x, y, z});
				}
				else
				{
					// else get the block to the south.
					BlockType* south = blocks[Chunk::getVectorIndex(x, y, z + 1)];

					// if the block to the south is not solid, or is not a full block, add the south face.
					if (south->category != BlockCategory::SOLID || *blockRegistry->models.get(south->uniqueIdentifier) != BlockModel::BLOCK)
					{
						insertToMesh(BLOCK_BACK, BLOCK_FACE_VERT_COUNT, BlockFace::SOUTH, {x, y, z});
					}
				}

				// check if block is on the bottom border of the chunk.
				if (y == 0)
				{
					// if on bottom border, add bottom face.
					insertToMesh(BLOCK_BOTTOM, BLOCK_FACE_VERT_COUNT, BlockFace::BOTTOM, {x, y, z});
				}
				else
				{
					// else get the block underneath.
					BlockType* bottom = blocks[Chunk::getVectorIndex(x, y - 1, z)];

					if (bottom->category != BlockCategory::SOLID || *blockRegistry->models.get(bottom->uniqueIdentifier) != BlockModel::BLOCK)
					{
						insertToMesh(BLOCK_BOTTOM, BLOCK_FACE_VERT_COUNT, BlockFace::BOTTOM, {x, y, z});
					}
				}

				// check if the block is on the top border.
				if (y == Chunk::CHUNK_HEIGHT - 1)
				{
					insertToMesh(BLOCK_TOP, BLOCK_FACE_VERT_COUNT, BlockFace::TOP, {x, y, z});
				}
				else
				{
					// else get the block to the top of it.
					BlockType* top = blocks[Chunk::getVectorIndex(x, y + 1, z)];

					if (top->category != BlockCategory::SOLID || *blockRegistry->models.get(top->uniqueIdentifier) != BlockModel::BLOCK)
					{
						insertToMesh(BLOCK_TOP, BLOCK_FACE_VERT_COUNT, BlockFace::TOP, {x, y, z});
					}
				}

				if (x == 0)
				{
					insertToMesh(BLOCK_RIGHT, BLOCK_FACE_VERT_COUNT, BlockFace::EAST, {x, y, z});
				}
				else
				{
					BlockType* east = blocks[Chunk::getVectorIndex(x - 1, y, z)];

					if (east->category != BlockCategory::SOLID || *blockRegistry->models.get(east->uniqueIdentifier) != BlockModel::BLOCK)
					{
						insertToMesh(BLOCK_RIGHT, BLOCK_FACE_VERT_COUNT, BlockFace::EAST, {x, y, z});
					}
				}

				if (x == Chunk::CHUNK_WIDTH - 1)
				{
					insertToMesh(BLOCK_LEFT, BLOCK_FACE_VERT_COUNT, BlockFace::WEST, {x, y, z});
				}
				else
				{
					BlockType* west = blocks[Chunk::getVectorIndex(x + 1, y, z)];

					if (west->category != BlockCategory::SOLID || *blockRegistry->models.get(west->uniqueIdentifier) != BlockModel::BLOCK)
					{
						insertToMesh(BLOCK_LEFT, BLOCK_FACE_VERT_COUNT, BlockFace::WEST, {x, y, z});
					}
				}
			}
			break;
		case BlockModel::SLAB:
			insertToMesh(SLAB_FRONT,  SLAB_FACE_VERT_COUNT, BlockFace::NORTH,  {x, y, z});
			insertToMesh(SLAB_RIGHT,  SLAB_FACE_VERT_COUNT, BlockFace::EAST,   {x, y, z});
			insertToMesh(SLAB_BACK,   SLAB_FACE_VERT_COUNT, BlockFace::SOUTH,  {x, y, z});
			insertToMesh(SLAB_LEFT,   SLAB_FACE_VERT_COUNT, BlockFace::WEST,   {x, y, z});
			insertToMesh(SLAB_TOP,    SLAB_FACE_VERT_COUNT, BlockFace::TOP,    {x, y, z});
			insertToMesh(SLAB_BOTTOM, SLAB_FACE_VERT_COUNT, BlockFace::BOTTOM, {x, y, z});
			break;
		case BlockModel::SLOPE:
			insertToMesh(SLOPE_FRONT,  SLOPE_FRONT_COUNT,  BlockFace::NORTH,  {x, y, z});
			insertToMesh(SLOPE_RIGHT,  SLOPE_RIGHT_COUNT,  BlockFace::EAST,   {x, y, z});
			insertToMesh(SLOPE_BACK,   SLOPE_BACK_COUNT,   BlockFace::SOUTH,  {x, y, z});
			insertToMesh(SLOPE_BOTTOM, SLOPE_BOTTOM_COUNT, BlockFace::BOTTOM, {x, y, z});
			break;
		case BlockModel::STAIR:
			insertToMesh(STAIR_FRONT,  STAIR_FRONT_COUNT,  BlockFace::NORTH,  {x, y, z});
			insertToMesh(STAIR_RIGHT,  STAIR_RIGHT_COUNT,  BlockFace::EAST,   {x, y, z});
			insertToMesh(STAIR_BACK,   STAIR_BACK_COUNT,   BlockFace::SOUTH,  {x, y, z});
			insertToMesh(STAIR_LEFT,   STAIR_LEFT_COUNT,   BlockFace::WEST,   {x, y, z});
			insertToMesh(STAIR_TOP,    STAIR_TOP_COUNT,    BlockFace::TOP,    {x, y, z});
			insertToMesh(STAIR_BOTTOM, STAIR_BOTTOM_COUNT, BlockFace::BOTTOM, {x, y, z});
			break;
		case BlockModel::X_PANEL:
			{
				for (std::size_t q = 0; q < XPANEL_MAX_VERTS; ++q)
				{
					std::size_t texLayer = 0;
					if ((*tex).size() != 4)
					{
						texLayer = texTable.at((*tex)[0]);
					}
					else
					{
						texLayer = texTable.at((*tex)[q / 6]);
					}

					phx::gfx::DefaultMeshVertex const* current = XPANEL_MESH + q;

					mesh.push_back(current->pos.x + (x * DEFAULT_MODEL_SIZE) + (chunkPos.x * DEFAULT_MODEL_SIZE));
					mesh.push_back(current->pos.y + (y * DEFAULT_MODEL_SIZE) + (chunkPos.y * DEFAULT_MODEL_SIZE));
					mesh.push_back(current->pos.z + (z * DEFAULT_MODEL_SIZE) + (chunkPos.z * DEFAULT_MODEL_SIZE));

					mesh.push_back(current->uv.x);
					mesh.push_back(current->uv.y);
					mesh.push_back(texLayer);

					mesh.push_back(current->normal.x);
					mesh.push_back(current->normal.y);
					mesh.push_back(current->normal.z);
				}
			}
			break;
		case BlockModel::X_PANEL_CUBE:
			{
				for (std::size_t q = 0; q < XPANEL_BLOCK_PANEL_VERT_COUNT; ++q)
				{
					std::size_t texLayer = 0;
					if ((*tex).size() != 10)
					{
						texLayer = texTable.at((*tex)[0]);
					}
					else
					{
						// int to int division will round down and make it return the value at either 1 to 4.
						texLayer = texTable.at((*tex)[static_cast<std::size_t>(q / 6)]);
					}

					phx::gfx::DefaultMeshVertex const* current = XPANEL_BLOCK_MESH + q;

					mesh.push_back(current->pos.x + (x * DEFAULT_MODEL_SIZE) + (chunkPos.x * DEFAULT_MODEL_SIZE));
					mesh.push_back(current->pos.y + (y * DEFAULT_MODEL_SIZE) + (chunkPos.y * DEFAULT_MODEL_SIZE));
					mesh.push_back(current->pos.z + (z * DEFAULT_MODEL_SIZE) + (chunkPos.z * DEFAULT_MODEL_SIZE));

					mesh.push_back(current->uv.x);
					mesh.push_back(current->uv.y);
					mesh.push_back(texLayer);

					mesh.push_back(current->normal.x);
					mesh.push_back(current->normal.y);
					mesh.push_back(current->normal.z);
				}

				for (std::size_t q = 0; q < XPANEL_BLOCK_BLOCK_VERT_COUNT; ++q)
				{
					std::size_t texLayer = 0;
					if ((*tex).size() != 10)
					{
						texLayer = texTable.at((*tex)[0]);
					}
					else
					{
						// divide by 6 to get face in block, +4 because first 4 textures should be for the xpanel.
						texLayer = texTable.at((*tex)[(XPANEL_BLOCK_BLOCK_VERT_COUNT + 24) / 6]);
					}

					phx::gfx::DefaultMeshVertex const* current = XPANEL_BLOCK_BLOCK_MESH + q;

					mesh.push_back(current->pos.x + (x * DEFAULT_MODEL_SIZE) + (chunkPos.x * DEFAULT_MODEL_SIZE));
					mesh.push_back(current->pos.y + (y * DEFAULT_MODEL_SIZE) + (chunkPos.y * DEFAULT_MODEL_SIZE));
					mesh.push_back(current->pos.z + (z * DEFAULT_MODEL_SIZE) + (chunkPos.z * DEFAULT_MODEL_SIZE));

					mesh.push_back(current->uv.x);
					mesh.push_back(current->uv.y);
					mesh.push_back(texLayer);

					mesh.push_back(current->normal.x);
					mesh.push_back(current->normal.y);
					mesh.push_back(current->normal.z);
				}
			}
			break;
		default:
			break;
		}
		// clang-format on
	}

	return mesh;
}
