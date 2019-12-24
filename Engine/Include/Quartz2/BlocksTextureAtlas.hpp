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

#include <Quartz2/Rect.hpp>
#include <Quartz2/OpenGL32.hpp>

#include <cstddef>
#include <unordered_map>

namespace q2
{
	/**
	 * Represents an group of textures, as an OpenGL texture array.
	 */
	class BlockTextureAtlas
	{
	public:
		/// A SpriteID represents a single texture within an atlas.
		using        SpriteID               = int;

		/// Value of an invalid SpriteID.
		static const SpriteID INVALID_SPRITE = -1;

		/**
		 * Constructor for setting the width and height of each texture in the atlas.
		 */
		BlockTextureAtlas(std::size_t spriteWidth, std::size_t spriteHeight);

		/**
		 * Default constructor. Initializes sprite width and height to zero.
		 */
		BlockTextureAtlas();

		/**
		 * @brief Register a texture as part of the atlas. Must be called before
		 *        patch() is called, in order to add all the textures.
		 *
		 * @param texturefilepath The filepath of the texture file to add to the atlas.
		 */
		void addTextureFile(const char* texturefilepath);

		/**
		 * @brief Combine all the textures added by addTextureFile into an
		 *        OpenGL texture array (accessible via getTextureArrayID()).
		 */
		void patch();

		/**
		 * @brief Set the width of the textures within the array.
		 *
		 * @param Width of the textures (pixels). Must be greater than 0.
		 */
		void setSpriteWidth(std::size_t w);

		/**
		 * @brief Set the height of the textures within the array.
		 *
		 * @param h Height of the textures (pixels). Must be greater than 0.
		 */
		void setSpriteHeight(std::size_t h);

		/**
		 * @brief Get the width of the textures within the atlas.
		 *
		 * @return Width of individual textures within the atlas (pixels).
		 */
		std::size_t getSpriteWidth() const { return m_spriteWidth; }

		/**
		 * @brief Get the height of the textures within the atlas.
		 *
		 * @return Height of individual textures within the atlas (pixels).
		 */
		std::size_t getSpriteHeight() const { return m_spriteHeight; }

		/**
		 * @brief Get the SpriteID for the texture at the specified filepath.
		 *        Calls to this function are only valid after patch() has been called.
		 *        filepath must have been added with addTextureFile.
		 *
		 * @param filepath The filepath of the texture to get the ID for.
		 * @return The assigned ID of the specified texture.
		 */
		SpriteID    getSpriteIDFromFilepath(const char* filepath);

		/**
		 * @brief Get the OpenGL ID of the texture array. Valid only after
		 *        patch() has been called.
		 *
		 * @return OpenGL ID of the texture array (generated by patch())
		 */
		OpenGL32::Object getTextureArrayID() const { return m_textureArray; }

	private:
		/// Map of texture filepath to ID - invalid until patch() is called.
		std::unordered_map<std::string, SpriteID> m_textureIDMap;

		/// Width of the textures within the array (pixels).
		std::size_t                               m_spriteWidth;

		/// Height of the textures within the array (pixels).
		std::size_t                               m_spriteHeight;

		/// OpenGL ID of the texture array. Undefined until patch() is called.
		OpenGL32::Object                          m_textureArray;
	};
}
