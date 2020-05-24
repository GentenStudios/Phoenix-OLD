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

#include <Common/Logger.hpp>

#include <glad/glad.h>

using namespace phx::gui;

Rectangle::Rectangle(Container* container, phx::math::vec2 pos, phx::math::vec2 size,
                     phx::math::vec3 color, float alpha, bool hasParent)
    : IComponent(container), m_pos(pos), m_size(size)
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

	/*
	 * The method used to generate the vertices here aren't the most efficient,
	 * we can combine a lot of the calculations at an earlier stage but it is
	 * all spread out to aid the future maintenance and reading of this code.
	 */

	phx::math::vec2 containerPos  = container->getPosition();
	phx::math::vec2 containerSize = container->getSize();

	// don't remove redundant brackets, they help the math flow while reading
	// since some people tend to forget how it works... -_-
	//
	// linearly interpolate to convert relative x position in the container to
	// an absolute x position within the window (still a percentage).
	phx::math::vec2 relativePos = (containerPos - (containerSize / 2.f)) +
	                         ((m_pos / 100.f) * containerSize);

	// relativePos will still be between 0 and 100, so we need divide by 100.
	relativePos /= 100.f;
	relativePos -= 0.5;

	// get the size relative to the parent container.
	// this way, if you have:
	// 100 / 100 = 1.f
	// 1.f * container size = the container size.
	// 0.5f * container size = half the container size.
	// it's a directly proportional value.
	phx::math::vec2 relativeSize = (m_size / 100.f) * (containerSize / 100.f) * 2.f;

	// calculate top left, and calculate everything from there.
	phx::math::vec2 topLeft = {relativePos.x - relativeSize.x / 2.f,
	                      relativePos.y + relativeSize.y / 2.f};

	phx::math::vec2 topRight = {topLeft.x + relativeSize.x, topLeft.y};

	phx::math::vec2 bottomRight = {topRight.x, topRight.y - relativeSize.y};

	phx::math::vec2 bottomLeft = {topLeft.x, topLeft.y - relativeSize.y};

	m_vertices.push_back({topRight, color, alpha, {}});
	m_vertices.push_back({bottomRight, color, alpha, {}});
	m_vertices.push_back({topLeft, color, alpha, {}});
	m_vertices.push_back({bottomRight, color, alpha, {}});
	m_vertices.push_back({bottomLeft, color, alpha, {}});
	m_vertices.push_back({topLeft, color, alpha, {}});

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
	             m_vertices.data(), GL_DYNAMIC_DRAW);

	// if it has a parent object, that object should be registered, not this
	// one, otherwise there could be duplicate actions occurring (like moving
	// things about, etc..)
	if (!hasParent)
	{
		container->attachComponent(this);
	}
}

Rectangle::~Rectangle()
{
	container->detachComponent(this);

	glDeleteBuffers(1, &m_buffer);
	glDeleteVertexArrays(1, &m_vao);
}

Vertex Rectangle::getCorner(Corner corner)
{
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

void Rectangle::setCorner(Corner corner, const Vertex& vertex)
{
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

bool Rectangle::isPointInObject(const phx::math::vec2& pos)
{
	phx::math::vec2 newPos = pos;
	newPos /= container->getWindow()->getSize();
	newPos -= 0.5f;

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

	phx::math::vec2 topRight    = m_vertices[0].vert;
	phx::math::vec2 bottomRight = m_vertices[1].vert;
	phx::math::vec2 bottomLeft  = m_vertices[4].vert;
	phx::math::vec2 topLeft     = m_vertices[2].vert;

	if (newPos.x >= topLeft.x && newPos.y <= topLeft.y)
	{
		if (newPos.x >= bottomLeft.x && newPos.y >= bottomLeft.y)
		{
			if (newPos.x <= topRight.x && newPos.y <= topRight.y)
			{
				if (newPos.x <= bottomRight.x && newPos.y >= bottomRight.y)
				{
					return true;
				}
			}
		}
	}

	return false;
}

phx::math::vec2 Rectangle::getPosition() const
{
	// returns a percentage, the percentage within the container that this
	// object is.
	return m_pos;
}

void Rectangle::setPosition(const phx::math::vec2& position)
{
	m_pos = position;

	///////////////////////////////////////////////////
	/// USE CODE FROM CONSTRUCTOR SINCE IT'S EASIER ///
	///////////////////////////////////////////////////

	/*
	 * The method used to generate the vertices here aren't the most efficient,
	 * we can combine a lot of the calculations at an earlier stage but it is
	 * all spread out to aid the future maintenance and reading of this code.
	 */

	phx::math::vec2 containerPos  = container->getPosition();
	phx::math::vec2 containerSize = container->getSize();

	// don't remove redundant brackets, they help the math flow while reading
	// since some people tend to forget how it works... -_-
	//
	// linearly interpolate to convert relative x position in the container to
	// an absolute x position within the window (still a percentage).
	phx::math::vec2 relativePos = (containerPos - (containerSize / 2.f)) +
	                         ((m_pos / 100.f) * containerSize);

	// relativePos will still be between 0 and 100, so we need divide by 100.
	relativePos /= 100.f;

	// get the size relative to the parent container.
	// this way, if you have:
	// 100 / 100 = 1.f
	// 1.f * container size = the container size.
	// 0.5f * container size = half the container size.
	// it's a directly proportional value.
	const phx::math::vec2 relativeSize = (m_size / 100.f) * (containerSize / 100.f) * 2.f;

	// converts the relative size and position into pixels.
	const phx::math::vec2 staticObjectPos =
	    relativePos - 0.5f /** container->getWindow()->getSize()*/;
	const phx::math::vec2 staticObjectSize =
	    relativeSize /** container->getWindow()->getSize()*/;

	// calculate top left, and calculate everything from there.
	const phx::math::vec2 topLeft = {staticObjectPos.x - staticObjectSize.x / 2.f,
	                            staticObjectPos.y + staticObjectSize.y / 2.f};

	const phx::math::vec2 topRight = {topLeft.x + staticObjectSize.x, topLeft.y};

	const phx::math::vec2 bottomRight = {topRight.x,
	                                topRight.y - staticObjectSize.y};

	const phx::math::vec2 bottomLeft = {topLeft.x, topLeft.y - staticObjectSize.y};

	m_vertices[0].vert = topRight;
	m_vertices[1].vert = bottomRight;
	m_vertices[3].vert = topLeft;
	m_vertices[1].vert = bottomRight;
	m_vertices[2].vert = bottomLeft;
	m_vertices[3].vert = topLeft;

	// upload this new data to the GPU.
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex),
	                m_vertices.data());
}

phx::math::vec2 Rectangle::getSize() const
{
	// returns size RELATIVE to the container.
	return m_size;
}

/// @todo fix this function!!!!
void Rectangle::setSize(const phx::math::vec2& size)
{
	if (size == m_size)
	{
		return;
	}

	// can be negative, so it just gets smaller or something.
	const auto difference = (size - m_size) / 100.f;

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

void Rectangle::onEvent(phx::events::Event& event) {}

void Rectangle::tick(float dt)
{
	// render vertices.
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
}
