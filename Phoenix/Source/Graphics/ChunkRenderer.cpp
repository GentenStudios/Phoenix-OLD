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

#include <Quartz2/Graphics/ChunkRenderer.hpp>
#include <Quartz2/Graphics/ShaderPipeline.hpp>
#include <Quartz2/Voxels/BlockRegistry.hpp>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Quartz2/Graphics/OpenGLTools.hpp"
#include <iostream>

using namespace q2;
using namespace gfx;

struct Vertex
{
	math::vec3 verts;
	math::vec3 uv; // includes tex layer
};

ChunkRenderer::ChunkRenderer(std::size_t visibleChunks)
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// 36 Verts per cube.
	// 16 * 16 * 16 cubes in each chunk = 4096;
	// 4096 * 36 = 147,456 verts
	// This assumes that every face and every vert is pumped to the GPU. Some
	// verts maybe empty but whatevs. THIS IS TEMPORARY.

	glGenBuffers(1, &m_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER,
	             sizeof(Vertex) * (36 * 16 * 16 * 16) * visibleChunks, nullptr,
	             GL_DYNAMIC_DRAW);

	glVertexAttribPointer(m_vertexAttributeLocation, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, verts)));

	// we're gonna pack texLayer into this cos why not.
	glVertexAttribPointer(m_uvAttributeLocation, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, uv)));

	glEnableVertexAttribArray(m_vertexAttributeLocation);
	glEnableVertexAttribArray(m_uvAttributeLocation);

	for (int i = 0; i < visibleChunks; ++i)
	{
		m_bigBufferLocations.push_back(i);
	}
}

ChunkRenderer::~ChunkRenderer() { glDeleteVertexArrays(1, &m_vao); }

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

ChunkRenderer::MeshIdentifier ChunkRenderer::submitChunkMesh(
    const std::vector<float>& mesh, MeshIdentifier slot)
{
	
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * 36 * 16 * 16 * 16 * slot,
	                sizeof(float) * mesh.size(), mesh.data());

	glBindVertexArray(0);

	m_multiDrawStarts.push_back(36 * 16 * 16 * 16 * slot);
	m_multiDrawCounts.push_back(static_cast<GLsizei>(mesh.size()));

	return 1;
}

void ChunkRenderer::render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);

	glBindVertexArray(m_vao);
	glMultiDrawArrays(GL_TRIANGLES, &m_multiDrawStarts[0],
	                  &m_multiDrawCounts[0], m_multiDrawStarts.size());
}
