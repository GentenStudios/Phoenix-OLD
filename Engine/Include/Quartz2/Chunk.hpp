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

#include <Quartz2/Chunk.hpp>
#include <Quartz2/Mesh.hpp>

#include <cstddef>
#include <memory>

namespace q2
{
	enum EChunkBlockType
	{
		CHUNK_BLOCK_TYPE_AIR,
		CHUNK_BLOCK_TYPE_DIRT
	};

	struct ChunkBlock
	{
		EChunkBlockType type;
	};

	class Chunk
	{
	public:
		Chunk();

		static constexpr std::size_t NUM_BLOCKS = 16 * 16 * 16;

		std::shared_ptr<Mesh> generateMesh();

		EChunkBlockType getBlockTypeToLeft(std::size_t x, std::size_t y, std::size_t z);
		EChunkBlockType getBlockTypeToRight(std::size_t x, std::size_t y, std::size_t z);
		EChunkBlockType getBlockTypeToTop(std::size_t x, std::size_t y, std::size_t z);
		EChunkBlockType getBlockTypeToBottom(std::size_t x, std::size_t y, std::size_t z);
		EChunkBlockType getBlockTypeToFront(std::size_t x, std::size_t y, std::size_t z);
		EChunkBlockType getBlockTypeToBack(std::size_t x, std::size_t y, std::size_t z);

	private:
		std::vector<ChunkBlock> m_blocks;
	};
}