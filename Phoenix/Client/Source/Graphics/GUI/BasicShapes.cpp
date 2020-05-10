// Copyright 2019-2020 Genten Studios
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

#include <Client/Graphics/GUI/BasicShapes.hpp>
#include <Client/Graphics/GUI/Container.hpp>

#include <glad/glad.h>

using namespace phx::gui;

Rectangle::Rectangle()
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_buffer);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, vert)));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, color)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, uv)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

Rectangle::~Rectangle()
{
	glDeleteBuffers(1, &m_buffer);
	glDeleteVertexArrays(1, &m_vao);
}

void Rectangle::init(math::vec2 position, math::vec2 size)
{
	m_vertices.clear();
	
	// position relative to the container.
	auto containerPosition = position / 100.f;
	containerPosition *= container->getPosition();

	auto containerSize = size / 100.f;
	containerSize *= container->getSize();

	math::vec2 topRight = {containerPosition.x + (containerSize.x / 2.f),
	                       containerPosition.y + (containerSize.y / 2.f)};

	math::vec2 bottomRight = {containerPosition.x + (containerSize.x / 2.f),
	                       containerPosition.y - (containerSize.y / 2.f)};

	math::vec2 bottomLeft = {containerPosition.x - (containerSize.x / 2.f),
	                          containerPosition.y - (containerSize.y / 2.f)};

	math::vec2 topLeft = {containerPosition.x - (containerSize.x / 2.f),
	                         containerPosition.y + (containerSize.y / 2.f)};

	m_vertices.push_back({topRight, {1.f, 1.f, 1.f}, 1.f, {}});
	m_vertices.push_back({bottomRight, {1.f, 1.f, 1.f}, 1.f, {}});
	m_vertices.push_back({topLeft, {1.f, 1.f, 1.f}, 1.f, {}});
	m_vertices.push_back({bottomRight, {1.f, 1.f, 1.f}, 1.f, {}});
	m_vertices.push_back({bottomLeft, {1.f, 1.f, 1.f}, 1.f, {}});
	m_vertices.push_back({topLeft, {1.f, 1.f, 1.f}, 1.f, {}});

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size(), m_vertices.data(),
	             GL_DYNAMIC_DRAW);
}

Vertex Rectangle::getCorner(Corner corner)
{
	if (m_vertices.size() >= static_cast<std::size_t>(corner) + 1)
	{
		return m_vertices[static_cast<std::size_t>(corner)];
	}

	return {};
}

void Rectangle::setCorner(Corner corner, Vertex vertex)
{
	if (m_vertices.size() != 6)
	{
		m_vertices.resize(6);
	}

	m_vertices[static_cast<std::size_t>(corner)] = vertex;
}

void Rectangle::update(math::vec2 moved, math::vec2 scale)
{
	// @todo THIS ALL NEEDS TESTING
	if (m_vertices.size() != 6)
	{
		return;
	}

	for (auto& vertex : m_vertices)
	{
		if (scale.x != 1.f && scale.y != 1.f)
		{
//			math::vec2 changeInPos = m_pos - (m_pos * scale);
		}
	}
}


bool Rectangle::isPointInObject(math::vec2 pos)
{
	if (m_vertices.empty())
	{
		return false;
	}	
	
	return false;
}

void Rectangle::tick(float dt)
{
	if (m_vertices.empty())
	{
		return;
	}

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
}
