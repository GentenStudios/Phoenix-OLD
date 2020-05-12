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

Rectangle::Rectangle(Container* container, math::vec2 pos, math::vec2 size,
                     math::vec3 color, float alpha,
                     Mode mode)
    : IComponent(container)
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

	math::vec2 relativePos;
	math::vec2 relativeSize;

	if (mode == Mode::RELATIVE)
	{
		m_pos  = pos / 100.f;
		m_size = size / 100.f;

		// take away 0.5 since opengl likes -1 to 1.
		// relativePos  = container->getPosition(Mode::RELATIVE);
		relativePos  = (m_pos - 0.5f) * container->getPosition(Mode::RELATIVE);
		relativeSize = m_size * container->getSize(Mode::RELATIVE);
	}
	else
	{
		relativePos  = pos / container->getWindow()->getSize();
		relativeSize = size / container->getWindow()->getSize();

		// convert and store positions relative to the CONTAINER, instead of
		// window.
		m_pos  = relativePos / container->getPosition(Mode::RELATIVE);
		m_size = relativeSize / container->getPosition(Mode::RELATIVE);
	}

	math::vec2 topRight = {
	    relativePos.x + relativeSize.x / 2.f,
	    relativePos.y + relativeSize.y / 2.f,
	};

	math::vec2 bottomRight = {
	    relativePos.x + relativeSize.x / 2.f,
	    relativePos.y - relativeSize.y / 2.f,
	};

	math::vec2 bottomLeft = {
	    relativePos.x - relativeSize.x / 2.f,
	    relativePos.y - relativeSize.y / 2.f,
	};

	math::vec2 topLeft = {
	    relativePos.x - relativeSize.x / 2.f,
	    relativePos.y + relativeSize.y / 2.f,
	};

	m_vertices.push_back({topRight,    color, alpha, {}});
	m_vertices.push_back({bottomRight, color, alpha, {}});
	m_vertices.push_back({topLeft,     color, alpha, {}});
	m_vertices.push_back({bottomRight, color, alpha, {}});
	m_vertices.push_back({bottomLeft,  color, alpha, {}});
	m_vertices.push_back({topLeft,     color, alpha, {}});

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
	             m_vertices.data(), GL_DYNAMIC_DRAW);

	container->attachComponent(this);
}

Rectangle::~Rectangle()
{
	container->detachComponent(this);

	glDeleteBuffers(1, &m_buffer);
	glDeleteVertexArrays(1, &m_vao);
}

Vertex Rectangle::getCorner(Corner corner)
{
	if (m_vertices.size() != 6)
	{
		return {};
	}

	/**
	 * 1st OpenGL Vertex: Top Right     (0)
	 * 2nd OpenGL Vertex: Bottom Right  (1)
	 * 3rd OpenGL Vertex: Top Left      (2)
	 * 4th OpenGL Vertex: Bottom Right  (3)
	 * 5th OpenGL Vertex: Bottom Left   (4)
	 * 6th OpenGL Vertex: Top Left      (5)
	 *
	 * The numbers in the brackets are the positions within the vector.
	 * When setCorner is used, it sets all instances of a specific corner to
	 * the same thing, so we don't need need to determine which instance to
	 * return, just one of them.
	 */

	switch (corner)
	{
	case Corner::TOP_RIGHT:
		return m_vertices[0];
		break;
	case Corner::BOTTOM_RIGHT:
		return m_vertices[1];
		break;
	case Corner::BOTTOM_LEFT:
		return m_vertices[4];
		break;
	case Corner::TOP_LEFT:
		return m_vertices[5];
		break;
	default:
		return {};
		break;
	}
}

void Rectangle::setCorner(Corner corner, Vertex vertex)
{
	if (m_vertices.size() != 6)
	{
		m_vertices.resize(6);
	}

	/**
	 * 1st OpenGL Vertex: Top Right     (0)
	 * 2nd OpenGL Vertex: Bottom Right  (1)
	 * 3rd OpenGL Vertex: Top Left      (2)
	 * 4th OpenGL Vertex: Bottom Right  (3)
	 * 5th OpenGL Vertex: Bottom Left   (4)
	 * 6th OpenGL Vertex: Top Left      (5)
	 *
	 * The numbers in the brackets are the positions within the vector.
	 */

	// this is not the most serviceable code, but it's much less messy and the
	// above note should help.
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	switch (corner)
	{
	case Corner::TOP_RIGHT:
		m_vertices[0] = vertex;
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex), &m_vertices[0]);
		break;

	case Corner::BOTTOM_RIGHT:
		m_vertices[1] = vertex;
		glBufferSubData(GL_ARRAY_BUFFER, 1 * sizeof(Vertex), sizeof(Vertex),
		                &m_vertices[1]);
		m_vertices[3] = vertex;
		glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex), sizeof(Vertex),
		                &m_vertices[3]);
		break;

	case Corner::BOTTOM_LEFT:
		m_vertices[4] = vertex;
		glBufferSubData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), sizeof(Vertex),
		                &m_vertices[4]);
		break;

	case Corner::TOP_LEFT:
		m_vertices[2] = vertex;
		glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(Vertex), sizeof(Vertex),
		                &m_vertices[3]);
		m_vertices[5] = vertex;
		glBufferSubData(GL_ARRAY_BUFFER, 5 * sizeof(Vertex), sizeof(Vertex),
		                &m_vertices[5]);
		break;
	default:
		break;
	}
}

bool Rectangle::isPointInObject(math::vec2 pos)
{
	/// @todo implement to check whether a position is within the square.

	if (m_vertices.empty())
	{
		return false;
	}

	return false;
}

phx::math::vec2 Rectangle::getPosition(Mode mode)
{
	if (mode == Mode::RELATIVE)
	{
		// returns relative to the CONTAINER.
		return m_pos;
	}

	// relative position * container's relative position * window's actual size
	// gets the static size in pixels.
	return m_pos * container->getPosition(Mode::RELATIVE) *
	       container->getWindow()->getSize();
}

void Rectangle::setPosition(math::vec2 position, Mode mode)
{
	// vertices haven't actually been generated.
	if (m_vertices.size() != 6)
	{
		return;
	}

	if (mode == Mode::RELATIVE)
	{
		position /= 100.f;

		// don't process anything if the position's are equivalent.
		if (position == m_pos)
		{
			return;
		}

		// get the difference in position.
		position = position - m_pos;

		// get OpenGL-like coords, relative to the window (it becomes relative
		// to the window BECAUSE we multiply by the container position).
		position *= container->getPosition(Mode::RELATIVE);
	}
	else
	{
		// get position relative to window, skip container part since we're
		// using pixels.
		position /= container->getWindow()->getSize();

		// get current size, relative to the window rather than to the
		// container.
		auto currentRelativeToWindowSize =
		    m_pos * container->getSize(Mode::RELATIVE);

		if (position == currentRelativeToWindowSize)
		{
			return;
		}

		// get difference in pixels.
		position = currentRelativeToWindowSize - position;
	}

	// opengl likes -1 to 1 so we take away 0.5
	position -= 0.5f;

	// apply the transformation to each vertex.
	for (auto& vertex : m_vertices)
	{
		vertex.vert += position;
	}

	// upload this new data to the GPU.
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
	             m_vertices.data(), GL_DYNAMIC_DRAW);
}

phx::math::vec2 Rectangle::getSize(Mode mode)
{
	if (mode == Mode::RELATIVE)
	{
		// returns size RELATIVE to the container.
		return m_size;
	}

	// relative size * container's relative size * window's actual size gets the
	// static size in pixels.
	return m_size * container->getSize(Mode::RELATIVE) *
	       container->getWindow()->getSize();
}

void Rectangle::setSize(math::vec2 size, Mode mode)
{
	// vertices haven't actually been generated.
	if (m_vertices.size() != 6)
	{
		return;
	}

	if (mode == Mode::RELATIVE)
	{
		size /= 100.f;

		if (size == m_size)
		{
			return;
		}

		// can be negative, so it just gets smaller or something.
		const auto difference = size - m_size;

		// apply the tranformation to each vertex.
		for (auto& vertex : m_vertices)
		{
			vertex.vert += difference;
		}

		// upload this new data to the GPU.
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
		             m_vertices.data(), GL_DYNAMIC_DRAW);
	}
}

void Rectangle::onEvent(events::Event event) {}

void Rectangle::tick(float dt)
{
	// don't waste a draw call if there aren't any vertices.
	if (m_vertices.empty())
	{
		return;
	}

	// render vertices.
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
}
