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

#include <Client/Graphics/ChunkRenderer.hpp>
#include <Client/Graphics/OpenGLTools.hpp>

#include <Common/Voxels/BlockRegistry.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>

using namespace phx;
using namespace gfx;

#include <glm/glm.hpp>

// struct to help with laying out the data inside the buffers.
struct Vertex
{
	float posx, poxy, posz;
	float uvu, uvv;
	float tex;
	float normalx, normaly, normalz;
	float colorx, colory, colorz;
};

ChunkRenderer::ChunkRenderer(const std::size_t visibleChunks)
{
	m_buffers.reserve(visibleChunks);
}

ChunkRenderer::~ChunkRenderer()
{
	for (auto& buffer : m_buffers)
	{
		glDeleteBuffers(1, &buffer.second.buffer);
		glDeleteVertexArrays(1, &buffer.second.vao);
	}
}

std::vector<ShaderLayout> ChunkRenderer::getRequiredShaderLayout()
{
	std::vector<ShaderLayout> layout;
	layout.emplace_back("a_Vertex", 0);
	layout.emplace_back("a_UV", 1);

	return layout;
}

void ChunkRenderer::buildTextureArray()
{
	glGenTextures(1, &m_textureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 16, 16, 256, 0, GL_RGBA,
	             GL_UNSIGNED_BYTE, nullptr);

	std::vector<std::string> texturePaths =
	    voxels::BlockRegistry::get()->getTextures()->getTextures();

	std::size_t i = 0;
	for (std::string& path : texturePaths)
	{
		int            width = -1, height = -1, nbChannels = -1;
		unsigned char* image =
		    stbi_load(path.c_str(), &width, &height, &nbChannels, 0);

		if (image != nullptr)
		{
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 16, 16, 1, GL_RGBA,
			                GL_UNSIGNED_BYTE, image);

			m_textureTable.insert({path, i});
			++i;
		}
		else
		{
			std::cout << "[RENDERING][TEXTURES] The file: " << path
			          << " does not exist." << std::endl;
		}

		stbi_image_free(image);
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

const ChunkRenderer::AssociativeTextureTable& ChunkRenderer::getTextureTable()
    const
{
	return m_textureTable;
}

void ChunkRenderer::submitChunk(const std::vector<float>& mesh, math::vec3 pos)
{
	if (mesh.empty())
	{
		return;
	}

	unsigned int vao;
	unsigned int buf;
	unsigned int buf0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), mesh.data(),
	             GL_DYNAMIC_DRAW);

	glVertexAttribPointer(m_vertexAttributeLocation, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, posx)));

	// we're gonna pack texLayer into this cos why not.
	glVertexAttribPointer(m_uvAttributeLocation, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, uvu)));

	glVertexAttribPointer(
		m_normalAttribLoc,
		3,
		GL_FLOAT,
		GL_TRUE,
		sizeof(Vertex),
	    reinterpret_cast<void*>(offsetof(Vertex, normalx))
	);

	glVertexAttribPointer(
		m_colorAttribLoc,
		3,
		GL_FLOAT,
		GL_TRUE,
		sizeof(Vertex),
	    reinterpret_cast<void*>(offsetof(Vertex, colorx))
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_buffers.insert({pos, {vao, buf, mesh.size()}});
}

void ChunkRenderer::updateChunk(const std::vector<float>& mesh, math::vec3 pos)
{
	const auto it = m_buffers.find(pos);
	if (it == m_buffers.end())
	{
		submitChunk(mesh, pos);
		return;
	}

	glBindVertexArray(it->second.vao);
	glBindBuffer(GL_ARRAY_BUFFER, it->second.buffer);

	// If the amount of vertices are the same, then it's actually more
	// efficient to just sub the data. glBufferData does a "re-alloc then
	// copy" - effectively the equivalent of std::vector::clear() then
	// filling it with data. glBufferSubData is more like std::vector::[i] =
	// blah.
	if (mesh.size() == it->second.vertexCount)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * mesh.size(),
		                mesh.data());
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), mesh.data(),
		             GL_DYNAMIC_DRAW);
		it->second.vertexCount = mesh.size();
	}
}

void ChunkRenderer::dropChunk(math::vec3 pos) { m_buffers.erase(pos); }

void ChunkRenderer::render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);

	for (auto& buffer : m_buffers)
	{
		glBindVertexArray(buffer.second.vao);

		glEnableVertexAttribArray(m_vertexAttributeLocation);
		glEnableVertexAttribArray(m_uvAttributeLocation);
		glEnableVertexAttribArray(m_normalAttribLoc);
		glEnableVertexAttribArray(m_colorAttribLoc);


		glDrawArrays(GL_TRIANGLES, 0, buffer.second.vertexCount);

		glDisableVertexAttribArray(m_vertexAttributeLocation);
		glDisableVertexAttribArray(m_uvAttributeLocation);
		glDisableVertexAttribArray(m_normalAttribLoc);
		glDisableVertexAttribArray(m_colorAttribLoc);

		glBindVertexArray(0);
		
	}

	
}

