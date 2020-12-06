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

#pragma once

#include <Common/Math/Math.hpp>

#include <unordered_map>
#include <vector>

namespace phx::gfx
{
	enum class TextureSize
	{
		X16  = 16,
		X32  = 32,
		X64  = 64,
		X128 = 128,
		X256 = 256,
		X512 = 512,
		UNKNOWN
	};

	struct TextureData
	{
		std::size_t layer = 0;

		// add uvSize.y to bottomLeftUV to get topLeft.
		// add uvSize.x to bottomLeftUV to get bottomRight.
		math::vec2 bottomLeftUV;
		math::vec2 uvSize;
	};

	class TexturePacker
	{
	public:
		using Handle = std::size_t;

		// the maximum size of a texture for a block/item that can be loaded.
		// may be improved upon in the future.
		constexpr static unsigned int MAX_TEXTURE_SIZE = 512;

	public:
		TexturePacker();
		~TexturePacker();

		Handle             add(const std::string& path);
		const TextureData* getData(Handle handle) const;

		void pack();

		void activate(unsigned int slot);

	private:
		bool                                    m_packed = false;
		std::vector<std::string>                m_texturesToLoad;
		std::unordered_map<Handle, TextureData> m_loadedTexData;

		unsigned int m_textureID = 0;
	};
} // namespace phx::gfx
