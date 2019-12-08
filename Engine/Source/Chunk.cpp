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

#include <Quartz2/Chunk.hpp>

using namespace q2;

Chunk::Chunk()
{
	m_blocks.resize(NUM_BLOCKS);

	for (std::size_t i = 0; i < NUM_BLOCKS; ++i)
	{
		m_blocks[i].type = CHUNK_BLOCK_TYPE_DIRT;
	}
}

static void addBlockTopFace(Vec3 pos, Mesh* mesh)
{
	mesh->addVertex({ pos });
	mesh->addVertex({ { pos.x, pos.y, pos.z + 1.f } });
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z + 1.f } });

	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z + 1.f } });
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z } });
	mesh->addVertex({ pos });
}

static void addBlockBottomFace(Vec3 pos, Mesh* mesh)
{
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z } });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z + 1.f } });
	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z + 1.f } });

	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z + 1.f } });
	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z } });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z } });
}

static void addBlockBackFace(Vec3 pos, Mesh* mesh)
{
	mesh->addVertex({ { pos.x, pos.y, pos.z + 1.f } });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z + 1.f } });
	mesh->addVertex({ { pos.x + 1, pos.y - 1.f, pos.z + 1.f } });

	mesh->addVertex({ { pos.x + 1, pos.y - 1.f, pos.z + 1.f} });
	mesh->addVertex({ { pos.x + 1, pos.y, pos.z + 1.f} });
	mesh->addVertex({ { pos.x, pos.y, pos.z + 1.f } });
}

static void addBlockFrontFace(Vec3 pos, Mesh* mesh)
{
	mesh->addVertex({ { pos.x, pos.y, pos.z} });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z } });
	mesh->addVertex({ { pos.x + 1, pos.y - 1.f, pos.z } });

	mesh->addVertex({ { pos.x + 1, pos.y - 1.f, pos.z } });
	mesh->addVertex({ { pos.x + 1, pos.y, pos.z } });
	mesh->addVertex({ { pos.x, pos.y, pos.z } });
}

static void addBlockRightFace(Vec3 pos, Mesh* mesh)
{
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z } });
	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z } });
	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z + 1.f } });

	mesh->addVertex({ { pos.x + 1.f, pos.y - 1.f, pos.z + 1.f } });
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z + 1.f } });
	mesh->addVertex({ { pos.x + 1.f, pos.y, pos.z } });
}

static void addBlockLeftFace(Vec3 pos, Mesh* mesh)
{
	mesh->addVertex({ pos });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z } });
	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z + 1.f } });

	mesh->addVertex({ { pos.x, pos.y - 1.f, pos.z + 1.f } });
	mesh->addVertex({ { pos.x, pos.y, pos.z + 1.f } });
	mesh->addVertex({ pos });
}

std::shared_ptr<Mesh> Chunk::generateMesh()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	
	for (std::size_t i = 0; i < NUM_BLOCKS; ++i)
	{
		const std::size_t x = i % 16;
		const std::size_t y = i / (16 * 16);
		const std::size_t z = (i / 16) % 16;

		const Vec3 blockPos(x, y, z);

		if (getBlockTypeToBack(x, y, z) == CHUNK_BLOCK_TYPE_AIR)
			addBlockBackFace(blockPos, mesh.get());
		
		if (getBlockTypeToFront(x, y, z) == CHUNK_BLOCK_TYPE_AIR)
			addBlockFrontFace(blockPos, mesh.get());

		if (getBlockTypeToLeft(x, y, z) == CHUNK_BLOCK_TYPE_AIR)
			addBlockLeftFace(blockPos, mesh.get());

		if (getBlockTypeToRight(x, y, z) == CHUNK_BLOCK_TYPE_AIR)
			addBlockRightFace(blockPos, mesh.get());

		if (getBlockTypeToTop(x, y, z) == CHUNK_BLOCK_TYPE_AIR)
			addBlockTopFace(blockPos, mesh.get());

		if (getBlockTypeToBottom(x, y, z) == CHUNK_BLOCK_TYPE_AIR)
			addBlockBottomFace(blockPos, mesh.get());
	}

	return mesh;
}

#define FLATTEN_XYZ(x, y, z) ((x) + 16 * ((y) + 16 * (z)))

EChunkBlockType Chunk::getBlockTypeToLeft(std::size_t x, std::size_t y, std::size_t z)
{
	if (x <= 0)
		return CHUNK_BLOCK_TYPE_AIR;

	return m_blocks[FLATTEN_XYZ(x - 1, y, z)].type;
}

EChunkBlockType Chunk::getBlockTypeToRight(std::size_t x, std::size_t y, std::size_t z)
{
	if (x >= 15)
		return CHUNK_BLOCK_TYPE_AIR;

	return m_blocks[FLATTEN_XYZ(x + 1, y, z)].type;
}

EChunkBlockType q2::Chunk::getBlockTypeToTop(std::size_t x, std::size_t y, std::size_t z)
{
	if (y >= 15)
		return CHUNK_BLOCK_TYPE_AIR;

	return m_blocks[FLATTEN_XYZ(x , y + 1, z)].type;
}

EChunkBlockType q2::Chunk::getBlockTypeToBottom(std::size_t x, std::size_t y, std::size_t z)
{
	if (y <= 0)
		return CHUNK_BLOCK_TYPE_AIR;

	return m_blocks[FLATTEN_XYZ(x, y - 1, z)].type;
}

EChunkBlockType q2::Chunk::getBlockTypeToBack(std::size_t x, std::size_t y, std::size_t z)
{
	if (z >= 15)
		return CHUNK_BLOCK_TYPE_AIR;

	return m_blocks[FLATTEN_XYZ(x, y, z + 1)].type;
}

EChunkBlockType q2::Chunk::getBlockTypeToFront(std::size_t x, std::size_t y, std::size_t z)
{
	if (z <= 0)
		return CHUNK_BLOCK_TYPE_AIR;

	return m_blocks[FLATTEN_XYZ(x, y, z - 1)].type;
}