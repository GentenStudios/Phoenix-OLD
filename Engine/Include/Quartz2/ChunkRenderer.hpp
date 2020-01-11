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

#include <Quartz2/OpenGL32.hpp>
#include <Quartz2/Camera.hpp>

#include <memory>
#include <Quartz2/Mesh.hpp>

namespace q2
{
	enum EChunkRendererStatus
	{
		CHUNK_RENDERER_STATUS_READY,
		CHUNK_RENDERER_STATUS_BAD_SHADERS
	};

	namespace enums
	{
		const char* toString(EChunkRendererStatus status);
	}

	struct ChunkRendererMesh
	{
		GLuint vbo;
		GLuint vao;

		std::shared_ptr<Mesh> mesh;
	};

	class ChunkRenderer
	{
	public:
		void setup(std::size_t viewWidth, std::size_t viewHeight);
		void teardown();

		void render(Camera* camera);

		EChunkRendererStatus status() const
			{ return m_status; }

		bool isReady() const
			{ return m_status == CHUNK_RENDERER_STATUS_READY; }

		void addMesh(const std::shared_ptr<Mesh>& mesh);

		void setTexture(unsigned char* data, std::size_t w, std::size_t h);

	private:
		GLuint m_terrainShader;
		GLuint m_texture;

		std::size_t m_viewWidth;
		std::size_t m_viewHeight;

		EChunkRendererStatus m_status;

		std::vector<ChunkRendererMesh> m_meshes;
	};
}