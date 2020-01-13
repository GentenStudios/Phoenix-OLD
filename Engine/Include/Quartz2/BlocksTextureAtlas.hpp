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

#include <Quartz2/Math/Rect.hpp>

#include <cstddef>
#include <unordered_map>
#include <cstring>

namespace q2
{
	class BlockTextureAtlas
	{
	public:
		typedef int           SpriteID;
		const static SpriteID INVALID_SPRITE = -1;

		BlockTextureAtlas(std::size_t spriteWidth,
			std::size_t spriteHeight);
		BlockTextureAtlas();
		~BlockTextureAtlas();

		void addTextureFile(const char* texturefilepath);
		void patch();
		void setSpriteWidth(std::size_t w);
		void setSpriteHeight(std::size_t h);

		std::size_t getSpriteWidth() const { return m_spriteWidth; }
		std::size_t getSpriteHeight() const { return m_spriteHeight; }
		SpriteID    getSpriteIDFromFilepath(const char* filepath);

		std::size_t getPatchedTextureWidth() const
		{
			return m_patchedTextureWidth;
		}

		std::size_t getPatchedTextureHeight() const
		{
			return m_patchedTextureHeight;
		}

		unsigned char* getPatchedTextureData() const
		{
			return m_patchedTextureData;
		}

		RectAABB getSpriteFromID(SpriteID spriteId) const;

	private:
		std::unordered_map<std::string, SpriteID> m_textureIDMap;
		std::size_t    m_spriteWidth, m_spriteHeight;
		unsigned char* m_patchedTextureData;

		std::size_t m_patchedTextureWidth, m_patchedTextureHeight;
	};
}
