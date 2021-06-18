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

#include <Client/Graphics/ChunkMesher.hpp>
#include <Client/Graphics/ChunkRenderer.hpp>

#include <Common/PlayerView.hpp>

#include <glad/glad.h>

#include <unordered_set>

using namespace phx::gfx;

ChunkRenderer::ChunkRenderer(phx::client::BlockRegistry* blockRegistry)
    : m_blockRegistry(blockRegistry)
{
	// lets say you have a view distance of 10, so lets do 10x10x10 and
	// just say you're gonna have 100 chunks in view at a time. you'll be
	// able to have more obviously, but this is just a simple
	// optimisation.
	m_chunks.reserve(100);
	m_buffers.reserve(100);

    m_renderPipeline.prepare("Assets/SimpleWorld.vert",
                             "Assets/SimpleWorld.frag",
                             gfx::ChunkRenderer::getRequiredShaderLayout());

    m_renderPipeline.activate();

    const math::mat4 model;
    m_renderPipeline.setMatrix("u_model", model);
}

ChunkRenderer::~ChunkRenderer() { clear(); }

std::vector<ShaderLayout> ChunkRenderer::getRequiredShaderLayout()
{
	std::vector<ShaderLayout> layout;
	layout.emplace_back("a_Vertex", 0);
	layout.emplace_back("a_UV", 1);

	return layout;
}

void ChunkRenderer::prep()
{
	m_blockRegistry->texturePacker.pack();
}

ChunkRenderData ChunkRenderer::generate(std::vector<float> mesh)
{
    ChunkRenderData data = {};
    glGenVertexArrays(1, &data.vao);
    glBindVertexArray(data.vao);

    glGenBuffers(1, &data.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, data.vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), mesh.data(),
                 GL_DYNAMIC_DRAW);

    glVertexAttribPointer(m_vertexAttributeLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, pos)));

    // we're gonna pack texLayer into the UV struct cos why not.
    glVertexAttribPointer(m_uvAttributeLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glVertexAttribPointer(
        m_normalAttributeLocation, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(m_vertexAttributeLocation);
    glEnableVertexAttribArray(m_uvAttributeLocation);
    glEnableVertexAttribArray(m_normalAttributeLocation);

    data.vertexCount = mesh.size();
    return data;
}

void ChunkRenderer::add(phx::voxels::Chunk* chunk)
{
	const auto it = std::find(m_chunks.begin(), m_chunks.end(), chunk);
	if (it == m_chunks.end())
	{
		m_chunks.push_back(chunk);
	}
	else
	{
		// chunk already exists, lets do nothing.
		return;
	}

	auto mesh = ChunkMesher::mesh(chunk, m_blockRegistry);
	if (mesh.empty())
	{
		// the mesh is empty, don't bother with adding it or anything.
		return;
	}

	m_buffers.insert({chunk->getChunkPos(), generate(mesh)});
}

void ChunkRenderer::update(phx::voxels::Chunk* chunk)
{
	const auto it = std::find(m_chunks.begin(), m_chunks.end(), chunk);
	if (it == m_chunks.end())
	{
		// add a chunk if not exists for compatibility.
		add(chunk);
		return;
	}

	auto mesh = ChunkMesher::mesh(chunk, m_blockRegistry);

	// we can't just say return if the mesh is empty, since we might be emptying
	// a mesh (breaking the final block in a chunk so only air is left or
	// something)

	auto            bufferExist = m_buffers.find(chunk->getChunkPos());
	if (bufferExist == m_buffers.end())
	{
		// data does not exist on GPU, we gotta make it.
		// but check if the mesh is empty first.
		if (mesh.empty())
		{
			return;
		}

		m_buffers.insert({chunk->getChunkPos(), generate(mesh)});
	}
	else
	{
		// buffer already exists.
		glBindVertexArray(bufferExist->second.vao);
		glBindBuffer(GL_ARRAY_BUFFER, bufferExist->second.vbo);

		if (mesh.size() == bufferExist->second.vertexCount)
		{
			// if the vertex count is the same, don't reallocate the buffer,
			// just change the value - will save expensive reallocation.
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * mesh.size(),
			                mesh.data());
		}
		else
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(),
			             mesh.data(), GL_DYNAMIC_DRAW);

			bufferExist->second.vertexCount = mesh.size();
		}
	}
}

void ChunkRenderer::remove(phx::voxels::Chunk* chunk)
{
	const auto it = std::find(m_chunks.begin(), m_chunks.end(), chunk);
	if (it != m_chunks.end())
	{
		// chunks is found, lets do something.

		// delete the opengl buffers.
		const auto buffer = m_buffers.find((*it)->getChunkPos());
		if (buffer != m_buffers.end())
		{
			glDeleteBuffers(1, &buffer->second.vbo);
			glDeleteVertexArrays(1, &buffer->second.vao);
		}

		// remove the buffer and chunk from internal memory.
		m_buffers.erase((*it)->getChunkPos());
		m_chunks.erase(it);
	}
}

void ChunkRenderer::clear()
{
	for (auto& buffer : m_buffers)
	{
		glDeleteBuffers(1, &buffer.second.vbo);
		glDeleteVertexArrays(1, &buffer.second.vao);
	}
}

void ChunkRenderer::onMapEvent(const phx::voxels::MapEvent& mapEvent)
{
	m_mapEvents.push(mapEvent);
}

void ChunkRenderer::tick(
    entt::registry* registry,
    entt::entity entity,
    const math::mat4& projection,
    const float& dt)
{
    auto position = registry->get<Position>(entity);
	// temp, will change in the future, based on game time
    static math::vec3 lightdir(0.f, -1.f, 0.f);
    static float      time = 0.f;

    time += dt;

    lightdir.y = std::sin(time);
    lightdir.x = std::cos(time);

    m_renderPipeline.activate();
    m_renderPipeline.setInt("u_TexArray", 0);
    m_renderPipeline.setMatrix("u_view", position.getView());
    m_renderPipeline.setMatrix("u_projection", projection);
    m_renderPipeline.setFloat("u_AmbientStrength", 0.7f);
    m_renderPipeline.setVector3("u_LightDir", lightdir);
    m_renderPipeline.setFloat("u_Brightness", 0.6f);

	for (auto& chunk : PlayerView::update(registry, entity))
	{
		add(chunk);
	}

	voxels::MapEvent e;
	while (m_mapEvents.try_pop(e))
	{
		if (e.type == voxels::MapEvent::CHUNK_UPDATE)
		{
			update(std::get<voxels::Chunk*>(e.data));
		}
	}
	
	m_blockRegistry->texturePacker.activate(0);

	for (auto& buffer : m_buffers)
	{
		glBindVertexArray(buffer.second.vao);
		glDrawArrays(GL_TRIANGLES, 0, buffer.second.vertexCount);
	}
}

