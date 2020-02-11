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

#include <Phoenix/CoreIntrinsics.hpp>
#include <Phoenix/Math/Math.hpp>
#include <Phoenix/Voxels/Block.hpp>

#include <vector>

namespace phx::voxels
{
    enum class BlockFace : int
    {
        FRONT  = 0,
        LEFT   = 1,
        BACK   = 2,
        RIGHT  = 3,
        TOP    = 4,
        BOTTOM = 5,
    };

    class Chunk
    {
    public:
        Chunk() = delete;

        explicit Chunk(math::vec3 chunkPos);
        ~Chunk()                  = default;
        Chunk(const Chunk& other) = default;
        Chunk& operator=(const Chunk& other) = default;
        Chunk(Chunk&& other) noexcept        = default;
        Chunk& operator=(Chunk&& other) noexcept = default;

        Chunk(math::vec3 chunkPos, const std::string& save);

        std::string save();

        void autoTestFill();

        math::vec3               getChunkPos() const;
        std::vector<BlockType*>& getBlocks();

        BlockType* getBlockAt(math::vec3 position) const;
        void       setBlockAt(math::vec3 position, BlockType* newBlock);

        static constexpr int CHUNK_WIDTH  = 16;
        static constexpr int CHUNK_HEIGHT = 16;
        static constexpr int CHUNK_DEPTH  = 16;

        static std::size_t getVectorIndex(std::size_t x, std::size_t y,
                                          std::size_t z)
        {
            return x + CHUNK_WIDTH * (y + CHUNK_HEIGHT * z);
        }

        ENGINE_FORCE_INLINE static std::size_t getVectorIndex(
            math::vec3 pos)
        {
            return getVectorIndex(static_cast<std::size_t>(pos.x),
                                  static_cast<std::size_t>(pos.y),
                                  static_cast<std::size_t>(pos.z));
        }

    private:
        math::vec3              m_pos;
        std::vector<BlockType*> m_blocks;
    };
} // namespace phx::voxels