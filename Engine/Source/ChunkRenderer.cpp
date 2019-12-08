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

void ChunkRenderer::setup()
{
	// Optimism :D (this will get changed later if anything does fail).
	m_status = CHUNK_RENDERER_STATUS_READY;

	const std::optional<GLuint> shader = ShaderCompiler::compile("Assets/TerrainShader.vert", "Assets/TerrainShader.frag");

	if (shader)
		m_terrainShader = shader.value();
	else	
		m_status = CHUNK_RENDERER_STATUS_BAD_SHADERS;

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	glUseProgram(m_terrainShader);
	
	glBindAttribLocation(m_terrainShader, 0, "aPos");

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void ChunkRenderer::teardown()
{
	// Bear in mind that the renderer may not be in a valid state when
	// it's being destroyed - we should do the best we can.
	if (isReady())
	{
		glDeleteProgram(m_terrainShader);
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
	}
}

void ChunkRenderer::render()
{
	glUseProgram(m_terrainShader);
	glBindVertexArray(m_vao);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}