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

#include <Client/Graphics/OpenGLTools.hpp>
#include <Client/Graphics/TexturePacker.hpp>

#include <Common/Logger.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <stb_image.h>

#include <algorithm>
#include <cmath>

using namespace phx::gfx;

TexturePacker::TexturePacker()
{
	// placeholder in case a handle doesn't exist.
	m_loadedTexData[0] = {0, {}, {}};
}

TexturePacker::~TexturePacker()
{
	if (m_packed)
	{
		glDeleteTextures(1, &m_textureID);
	}
}

TexturePacker::Handle TexturePacker::add(const std::string& path)
{
	if (m_packed)
		return 0;

	const auto it =
	    std::find(m_texturesToLoad.begin(), m_texturesToLoad.end(), path);
	if (it == m_texturesToLoad.end())
	{
		m_texturesToLoad.push_back(path);

		// note: handle starts at one, this lets us place a placeholder at index
		// 0 in the loaded array.
		return m_texturesToLoad.size();
	}

	// if the element already exists, return the position of it.
	return std::distance(m_texturesToLoad.begin(), it) + 1;
}

const TextureData* TexturePacker::getData(Handle handle) const
{
	const auto it = m_loadedTexData.find(handle);
	if (it != m_loadedTexData.end())
	{
		return &it->second;
	}

	// this only happens if the handle is not found.
	// we know the underlying value is set to 0 all around from the constructor
	// and will just result in a black box being rendered.
	return &m_loadedTexData.at(0);
}

void TexturePacker::pack()
{
	std::unordered_map<TextureSize, std::vector<std::pair<Handle, std::string>>>
	    textures;

	// load file info.
	for (std::size_t i = m_texturesToLoad.size(); i > 0; --i)
	{
		// get the size and number of components in each file.
		int x, y, comp;
		if (stbi_info(m_texturesToLoad[i - 1].c_str(), &x, &y, &comp))
		{
			// if the width doesn't equal the height it's not square and thus
			// definitely not a power of two size.
			if (x != y)
			{
				LOG_WARNING("TEXTURING")
				    << "The texture: " << m_texturesToLoad[i - 1]
				    << " has an invalid size. The only "
				       "accepted block/item texture sizes are: 16x16, "
				       "32x32, 64x64, 128x128, 256x256, 512x512";

				// just skip this texture.
				continue;
			}

			// checking if the texture is a power of two size, we can get away
			// with checking only the width because at this point we know the
			// two are equal from above.
			TextureSize size;
			switch (x)
			{
			case 16:
				size = TextureSize::X16;
				break;
			case 32:
				size = TextureSize::X32;
				break;
			case 64:
				size = TextureSize::X64;
				break;
			case 128:
				size = TextureSize::X128;
				break;
			case 256:
				size = TextureSize::X256;
				break;
			case 512:
				size = TextureSize::X512;
				break;
			default:
				size = TextureSize::UNKNOWN;
				break;
			}

			if (size == TextureSize::UNKNOWN)
			{
				LOG_WARNING("TEXTURING")
				    << "The texture: " << m_texturesToLoad[i - 1]
				    << " has an invalid size. The only "
				       "accepted block/item texture sizes are: 16x16, "
				       "32x32, 64x64, 128x128, 256x256, 512x512";

				// remove the texture from the list to prevent further
				// processing.
				m_texturesToLoad.pop_back();

				// just skip this texture.
				continue;
			}

			textures[size].emplace_back(i, std::move(m_texturesToLoad.back()));
			m_texturesToLoad.pop_back();
		}
		else
		{
			// the texture could not be found.
			LOG_WARNING("TEXTURING")
			    << "The texture: " << m_texturesToLoad[i - 1]
			    << " could not be loaded, it may not exist "
			       "or it may be corrupted.";

			// remove the texture from the list to prevent further processing.
			m_texturesToLoad.pop_back();
		}
	}

	/**
	 * ACTUALLY LOADING TEXTURES INTO MEMORY NOW.
	 */

	// count the total textures that are ready to be loaded.
	std::size_t loadedTextures = 0;
	for (auto& set : textures)
	{
		loadedTextures += set.second.size();
	}

	// get the maximum amount of layers the graphics card supports in a 2D array
	// texture.
	GLint maxLayers = 0;
	GLCheck(glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxLayers));

	// there are 6 supported sizes, lets count how many of each.
	const std::size_t count16  = textures[TextureSize::X16].size();
	const std::size_t count32  = textures[TextureSize::X32].size();
	const std::size_t count64  = textures[TextureSize::X64].size();
	const std::size_t count128 = textures[TextureSize::X128].size();
	const std::size_t count256 = textures[TextureSize::X256].size();
	const std::size_t count512 = textures[TextureSize::X512].size();

	// each layer is going to be 1024x1024 in size so we know:
	// you can fit 4096 16px textures on each one
	// you can fit 1024 32px textures on each one
	// you can fit 256 64px textures on each one
	// you can fit 64 128px textures on each one
	// you can fit 16 256px textures on each one
	// you can fit 4 512px textures on each one.
	// A modern GPU supporting OpenGL 4 will allow for 2048 layers, so even
	// with 512px textures, we could store 8,192 textures - which is a lot
	// (base minecraft contains about 2000 textures afaik)

	// this is not the most efficient way to do it, however it's by far the
	// easiest and most convenient way of doing it. this can and should be
	// revised in the future.

	std::size_t layersNeeded = 0;

	if (count16 > 0)
	{
		const float count = static_cast<float>(count16) / 4096.f;
		layersNeeded += static_cast<std::size_t>(std::ceil(count));
	}

	if (count32 > 0)
	{
		const float count = static_cast<float>(count32) / 1024.f;
		layersNeeded += static_cast<std::size_t>(std::ceil(count));
	}

	if (count64 > 0)
	{
		const float count = static_cast<float>(count64) / 256.f;
		layersNeeded += static_cast<std::size_t>(std::ceil(count));
	}

	if (count128 > 0)
	{
		const float count = static_cast<float>(count128) / 64.f;
		layersNeeded += static_cast<std::size_t>(std::ceil(count));
	}

	if (count256 > 0)
	{
		const float count = static_cast<float>(count256) / 16.f;
		layersNeeded += static_cast<std::size_t>(std::ceil(count));
	}

	if (count512 > 0)
	{
		const float count = static_cast<float>(count512) / 4.f;
		layersNeeded += static_cast<std::size_t>(std::ceil(count));
	}

	if (layersNeeded > static_cast<std::size_t>(maxLayers))
	{
		LOG_FATAL("TEXTURING") << "A texturing limit has been reached, these "
		                          "many textures cannot be loaded.";

		exit(EXIT_FAILURE);
	}

	GLCheck(glGenTextures(1, &m_textureID));
	GLCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID));

	// this is a hack that will make sure we get the error we want even in a
	// release build. we clear the opengl error queue so we can see if the next
	// error is something like an out of memory error.
	while (glGetError() != GL_NO_ERROR)
	{
	}

	// don't wrap this in GLCheck because we want to manually check for an out
	// of memory error.
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 1024, 1024,
	             static_cast<int>(layersNeeded), 0, GL_RGBA, GL_UNSIGNED_BYTE,
	             nullptr);

	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR)
	{
		if (err == GL_OUT_OF_MEMORY)
		{
			LOG_FATAL("TEXTURING") << "Out of memory to load textures into "
			                          "video memory. Aborting.";
			exit(EXIT_FAILURE);
		}

		LOG_FATAL("TEXTURING")
		    << "An unknown error occured while trying to load textures "
		       "into video memory. Aborting.";
		exit(EXIT_FAILURE);
	}

	// flip all images vertically on load, otherwise everything loads upside
	// down and i'm not bothering changing the UV system :(
	stbi_set_flip_vertically_on_load(true);

	std::size_t layer = 0;
	for (auto& size : textures)
	{
		while (!size.second.empty())
		{
			for (std::size_t y = 0;
			     y < 1024 / static_cast<std::size_t>(size.first); ++y)
			{
				if (size.second.empty())
					break;

				for (std::size_t x = 0;
				     x < 1024 / static_cast<std::size_t>(size.first); ++x)
				{
					if (size.second.empty())
						break;

					const std::size_t posX =
					    x * static_cast<std::size_t>(size.first);
					const std::size_t posY =
					    y * static_cast<std::size_t>(size.first);

					int            width, height, comp;
					unsigned char* image =
					    stbi_load(size.second.back().second.c_str(), &width,
					              &height, &comp, 4);

					if (image == nullptr)
					{
						LOG_WARNING("TEXTURING")
						    << "The texture: " << size.second.back().second
						    << " could not be loaded, either because it could "
						       "not "
						       "be found or it is corrupted.";

						continue;
					}

					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, posX, posY,
					                static_cast<int>(layer),
					                static_cast<int>(size.first),
					                static_cast<int>(size.first), 1, GL_RGBA,
					                GL_UNSIGNED_BYTE, image);

					stbi_image_free(image);

					// clang format makes this look like a rats arse.
					// clang-format off
					m_loadedTexData.insert({
						size.second.back().first,
						{
							layer,
							{static_cast<float>(posX) / 1024.f, static_cast<float>(posY) / 1024.f},
							{static_cast<float>(size.first) / 1024.f, static_cast<float>(size.first) / 1024.f}
						}
					});
					// clang-format on

					size.second.pop_back();
				}
			}

			++layer;
		}
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// this is shit, we need a proper way to setup texture filtering, but we can
	// implement this in the future.
	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso); 

	// @todo Manually generate mipmaps.

	m_packed = true;
}

void TexturePacker::activate(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
}
