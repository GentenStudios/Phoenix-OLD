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

#include <Quartz2/Chunk.hpp>

using namespace q2;

#define FLATTEN_XYZ(x, y, z) ((x) + 16 * ((y) + 16 * (z)))

Chunk::Chunk()
{
	m_blocks.resize(NUM_BLOCKS);

	BlockType* dirtBlock = BlockRegistry::get()->getBlockFromID("core:dirt");
	BlockType* grassBlock = BlockRegistry::get()->getBlockFromID("core:grass");
	BlockType* airBlock = BlockRegistry::get()->getBlockFromID("core:air");

	for (std::size_t i = 0; i < NUM_BLOCKS; i++)
	{
		const std::size_t y = i / (16 * 16);
		if (y < 15)
			m_blocks[i].type = dirtBlock;
		else
			m_blocks[i].type = grassBlock;
	}
}

static void addBlockTopFace(Vec3 pos, Mesh* mesh, BlockType* type)
{
	const RectAABB uvs = BlockRegistry::get()->getAtlas()->getSpriteFromID(type->textures.top);

	mesh->addVertex({ pos, uvs.topLeft });
	mesh->addVertex({ { pos.x, pos.y, pos.z + 1.f }, uvs.bottomLeft });
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z + 1.f }, uvs.bottomRight });

	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z + 1.f }, uvs.bottomRight });
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z }, uvs.topRight });
	mesh->addVertex({ pos , uvs.topLeft });
}

static void addBlockBottomFace(Vec3 pos, Mesh* mesh, BlockType* type)
{
	const RectAABB uvs = BlockRegistry::get()->getAtlas()->getSpriteFromID(type->textures.bottom);

	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z }, uvs.topLeft });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z + 1.f }, uvs.bottomLeft });
	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z + 1.f }, uvs.bottomRight });

	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z + 1.f }, uvs.bottomRight });
	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z }, uvs.topRight });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z }, uvs.topLeft });
}

static void addBlockBackFace(Vec3 pos, Mesh* mesh, BlockType* type)
{
	const RectAABB uvs = BlockRegistry::get()->getAtlas()->getSpriteFromID(type->textures.back);

	mesh->addVertex({ { pos.x, pos.y, pos.z + 1.f }, uvs.topLeft });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z + 1.f }, uvs.bottomLeft });
	mesh->addVertex({ { pos.x + 1, pos.y - 1.f, pos.z + 1.f }, uvs.bottomRight });

	mesh->addVertex({ { pos.x + 1, pos.y - 1.f, pos.z + 1.f}, uvs.bottomRight });
	mesh->addVertex({ { pos.x + 1, pos.y, pos.z + 1.f}, uvs.topRight });
	mesh->addVertex({ { pos.x, pos.y, pos.z + 1.f }, uvs.topLeft });
}

static void addBlockFrontFace(Vec3 pos, Mesh* mesh, BlockType* type)
{
	const RectAABB uvs = BlockRegistry::get()->getAtlas()->getSpriteFromID(type->textures.front);

	mesh->addVertex({ { pos.x, pos.y, pos.z },  uvs.topLeft});
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z }, uvs.bottomLeft });
	mesh->addVertex({ { pos.x + 1, pos.y - 1.f, pos.z }, uvs.bottomRight });

	mesh->addVertex({ { pos.x + 1, pos.y - 1.f, pos.z }, uvs.bottomRight });
	mesh->addVertex({ { pos.x + 1, pos.y, pos.z }, uvs.topRight });
	mesh->addVertex({ { pos.x, pos.y, pos.z }, uvs.topLeft });
}

static void addBlockRightFace(Vec3 pos, Mesh* mesh, BlockType* type)
{
	const RectAABB uvs = BlockRegistry::get()->getAtlas()->getSpriteFromID(type->textures.right);
	
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z }, uvs.topLeft });
	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z }, uvs.bottomLeft });
	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z + 1.f }, uvs.bottomRight });

	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z + 1.f }, uvs.bottomRight });
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z + 1.f }, uvs.topRight });
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z }, uvs.topLeft });
}

static void addBlockLeftFace(Vec3 pos, Mesh* mesh, BlockType* type)
{
	const RectAABB uvs = BlockRegistry::get()->getAtlas()->getSpriteFromID(type->textures.left);

	mesh->addVertex({ pos, uvs.topLeft });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z }, uvs.bottomLeft });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z + 1.f }, uvs.bottomRight });

	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z + 1.f }, uvs.bottomRight });
	mesh->addVertex({ { pos.x, pos.y, pos.z + 1.f }, uvs.topRight });
	mesh->addVertex({ pos, uvs.topLeft });
}

std::shared_ptr<Mesh> Chunk::generateMesh()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	
	for (std::size_t i = 0; i < NUM_BLOCKS; ++i)
	{
		const std::size_t x = i % 16;
		const std::size_t y = i / (16 * 16);
		const std::size_t z = (i / 16) % 16;

		if (m_blocks[FLATTEN_XYZ(x, y, z)].type->category == BLOCK_CATEGORY_AIR)
			continue;

		const Vec3 blockPos(x, y, z);

		BlockType* type = m_blocks[i].type;

		if (getCategoryOfBlockToBack(x, y, z) == BLOCK_CATEGORY_AIR)
			addBlockBackFace(blockPos, mesh.get(), type);
		
		if (getCategoryOfBlockToFront(x, y, z) == BLOCK_CATEGORY_AIR)
			addBlockFrontFace(blockPos, mesh.get(), type);

		if (getCategoryOfBlockToLeft(x, y, z) == BLOCK_CATEGORY_AIR)
			addBlockLeftFace(blockPos, mesh.get(), type);

		if (getCategoryOfBlockToRight(x, y, z) == BLOCK_CATEGORY_AIR)
			addBlockRightFace(blockPos, mesh.get(), type);

		if (getCategoryOfBlockToTop(x, y, z) == BLOCK_CATEGORY_AIR)
			addBlockTopFace(blockPos, mesh.get(), type);

		if (getCategoryOfBlockToBottom(x, y, z) == BLOCK_CATEGORY_AIR)
			addBlockBottomFace(blockPos, mesh.get(), type);
	}

	return mesh;
}


EBlockCategory Chunk::getCategoryOfBlockToLeft(std::size_t x, std::size_t y, std::size_t z)
{
	if (x <= 0)
		return BLOCK_CATEGORY_AIR;

	return m_blocks[FLATTEN_XYZ(x - 1, y, z)].type->category;
}

EBlockCategory Chunk::getCategoryOfBlockToRight(std::size_t x, std::size_t y, std::size_t z)
{
	if (x >= 15)
		return BLOCK_CATEGORY_AIR;

	return m_blocks[FLATTEN_XYZ(x + 1, y, z)].type->category;
}

EBlockCategory q2::Chunk::getCategoryOfBlockToTop(std::size_t x, std::size_t y, std::size_t z)
{
	if (y >= 15)
		return BLOCK_CATEGORY_AIR;

	return m_blocks[FLATTEN_XYZ(x , y + 1, z)].type->category;
}

EBlockCategory q2::Chunk::getCategoryOfBlockToBottom(std::size_t x, std::size_t y, std::size_t z)
{
	if (y <= 0)
		return BLOCK_CATEGORY_AIR;

	return m_blocks[FLATTEN_XYZ(x, y - 1, z)].type->category;
}

EBlockCategory q2::Chunk::getCategoryOfBlockToBack(std::size_t x, std::size_t y, std::size_t z)
{
	if (z >= 15)
		return BLOCK_CATEGORY_AIR;

	return m_blocks[FLATTEN_XYZ(x, y, z + 1)].type->category;
}

EBlockCategory q2::Chunk::getCategoryOfBlockToFront(std::size_t x, std::size_t y, std::size_t z)
{
	if (z <= 0)
		return BLOCK_CATEGORY_AIR;

	return m_blocks[FLATTEN_XYZ(x, y, z - 1)].type->category;
}