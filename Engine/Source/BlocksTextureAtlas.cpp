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

#include <Quartz2/BlocksTextureAtlas.hpp>

#include <algorithm>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace q2;

const BlockTextureAtlas::SpriteID BlockTextureAtlas::INVALID_SPRITE;

BlockTextureAtlas::BlockTextureAtlas(std::size_t spriteWidth, std::size_t spriteHeight)
	: m_spriteWidth(spriteWidth), m_spriteHeight(spriteHeight), m_textureArray(0)
{ }

BlockTextureAtlas::BlockTextureAtlas()
	: m_spriteWidth(0), m_spriteHeight(0), m_textureArray(0)
{ }

void BlockTextureAtlas::setSpriteWidth(std::size_t w)
{
	m_spriteWidth = w;
}

void BlockTextureAtlas::setSpriteHeight(std::size_t h)
{
	m_spriteHeight = h;
}

void BlockTextureAtlas::addTextureFile(const char* texturefilepath)
{
	const std::pair<std::string, SpriteID> sprite(texturefilepath, BlockTextureAtlas::INVALID_SPRITE);

	m_textureIDMap.insert(sprite);
}

BlockTextureAtlas::SpriteID BlockTextureAtlas::getSpriteIDFromFilepath(const char* filepath)
{
	const auto filepathEquality =
		[filepath](const std::unordered_map<std::string, SpriteID>::value_type a) -> bool
		{
			return a.first == filepath;
		};

	if (std::find_if(m_textureIDMap.begin(), m_textureIDMap.end(), filepathEquality) == m_textureIDMap.end())
		return BlockTextureAtlas::INVALID_SPRITE;

	return m_textureIDMap[filepath];
}

void BlockTextureAtlas::patch()
{
	assert(m_textureIDMap.size() <= GL_MAX_ARRAY_TEXTURE_LAYERS);

	// Cannot patch 0 textures into a array, so no point in even trying.
	if (m_textureIDMap.empty() || m_textureIDMap.size() > GL_MAX_ARRAY_TEXTURE_LAYERS)
		return;

	// Must have specified a non-zero sprite width and height before patching
	// the texture into the texture array.
	assert(m_spriteWidth != 0);
	assert(m_spriteHeight != 0);

	glGenTextures(1, &m_textureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);

	// Create the texture array, but don't fill it in with
	// anything - that'll be done by glTexSubImage3D calls
	// for each texture later.
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY,             // Texture Target.
		0,                               // LoD number - 0 is the base image level.
		GL_RGBA8,                        // Internal format - Number of color components.
		(GLsizei) m_spriteWidth,         // Width of the textures in the array (pixels).
		(GLsizei) m_spriteHeight,        // Height of the textures in the array (pixels).
		(GLsizei) m_textureIDMap.size(), // Number of textures in the array (depth/number of layers).
		0,                               // Border - Must be 0.
		GL_RGBA,                         // Format of the pixel data.
		GL_UNSIGNED_BYTE,                // Data type of the pixel data.
		0                                // Pointer to image data (0 for us, as it'll be filled in later)
	);
	
	std::size_t spriteIndex = 0;
	for (const auto& sprite : m_textureIDMap)
	{
		// Assign this texture an ID.
		const std::string& textureFilepath = sprite.first;
		m_textureIDMap[textureFilepath] = SpriteID(spriteIndex);

		int width       = -1;
		int height      = -1;
		int numChannels = -1;

		// Load the texture into memory (from disk). Required to be loaded in RGBA format.s
		unsigned char* image = stbi_load(textureFilepath.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

		// Basic sanity checks that the image has been loaded correctly.
		assert(image);
		assert(width > 0 && height > 0);
		assert(numChannels == 4);

		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY,      // Texture target.
			0,                        // LoD number - 0 is the base image level.
			0,                        // Texel X offset.
			0,                        // Texel Y offset.
			(GLint) spriteIndex,      // Texel Z offset (or the index of this texture in the array).
			(GLsizei) m_spriteWidth,  // Width of the (sub)image (pixels).
			(GLsizei) m_spriteHeight, // Height of the (sub)image (pixels).
			1,                        // Depth of the (sub)image (pixels).
			GL_RGBA,                  // Format of the pixel data.
			GL_UNSIGNED_BYTE,         // Data type of the pixel data.
			image                     // Pointer to the (sub)image pixel data.
		);

		// Release the allocated image data.
		stbi_image_free(image);

		spriteIndex++;
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Unbind the texture now we're done with it (for generation).
	// Not strictly necessary, but shouldn't have to much of a performance impact
	// here.
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
