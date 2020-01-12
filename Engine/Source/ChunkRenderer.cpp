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

#include <Quartz2/ChunkRenderer.hpp>
#include <Quartz2/ShaderCompiler.hpp>

using namespace q2;

const char* enums::toString(EChunkRendererStatus status)
{
	switch (status)
	{
	case CHUNK_RENDERER_STATUS_READY:
		return "CHUNK_RENDERER_STATUS_READY";
	case CHUNK_RENDERER_STATUS_BAD_SHADERS:
		return "CHUNK_RENDERER_STATUS_BAD_SHADERS";
	default:
		return "INVALID (EChunkRendererStatus)";
	}
}

void ChunkRenderer::addMesh(const std::shared_ptr<Mesh>& mesh)
{
	ChunkRendererMesh rendererMesh;
	rendererMesh.mesh = mesh;


	std::vector<Vertex>& vertices = mesh->getVertices();

	glGenVertexArrays(1, &rendererMesh.vao);
	glBindVertexArray(rendererMesh.vao);

	glBindVertexArray(rendererMesh.vao);

	glGenBuffers(1, &rendererMesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rendererMesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	m_meshes.push_back(rendererMesh);
}

void ChunkRenderer::setTexture(unsigned char* data, std::size_t w, std::size_t h)
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void ChunkRenderer::setup()
{
	// Optimism :D (this will get changed later if anything does fail).
	m_status = CHUNK_RENDERER_STATUS_READY;

	const std::optional<GLuint> shader = ShaderCompiler::compile("Assets/TerrainShader.vert", "Assets/TerrainShader.frag");

	if (shader)
		m_terrainShader = shader.value();
	else	
		m_status = CHUNK_RENDERER_STATUS_BAD_SHADERS;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);

	glUseProgram(m_terrainShader);
	glBindAttribLocation(m_terrainShader, 0, "in_Position");
}

void ChunkRenderer::teardown()
{
	// Bear in mind that the renderer may not be in a valid state when
	// it's being destroyed - we should do the best we can.
	if (isReady())
	{
		glDeleteProgram(m_terrainShader);
		
		for (ChunkRendererMesh& mesh : m_meshes)
		{
			glDeleteVertexArrays(1, &mesh.vao);
			glDeleteBuffers(1, &mesh.vbo);
		}
	}
}

void ChunkRenderer::render(gfx::FPSCamera* camera)
{
	glUseProgram(m_terrainShader);

	glBindTexture(GL_TEXTURE_2D, m_texture);
	glActiveTexture(GL_TEXTURE0);

	const math::mat4 view = camera->calculateViewMatrix()
	const math::mat4 proj = camera->getProjection()

	glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "u_view"), 1,
	                   GL_FALSE, &view[0]);
	glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "u_projection"), 1, GL_FALSE, &proj[0]);

	for (ChunkRendererMesh& mesh : m_meshes)
	{
		glBindVertexArray(mesh.vao);
		glDrawArrays(GL_TRIANGLES, 0, mesh.mesh->getVertices().size());
	}
}
