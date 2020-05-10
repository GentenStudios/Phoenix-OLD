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

#include <Client/Graphics/GUI/Container.hpp>

#include <glad/glad.h>

using namespace phx::gui;

Container::Container(const std::string& name, math::vec2 pos, math::vec2 size,
                     gfx::Window* window, Flags flags)
    : m_position(pos), m_size(size)
{
	const auto windowSize = window->getSize();

	// the size on the screen the element will be, opengl loves values between 0
	// and 1.
	const auto screenSize = (size / 100.f);

	// the amount across the screen the element will be, opengl loves values
	// between 0 and 1.
	const auto screenPos = (pos / 100.f) - 0.5f;

	// main box
	math::vec2 topRight = {screenPos.x + (screenSize.x / 2.f),
	                       screenPos.y + (screenSize.y / 2.f)};

	math::vec2 bottomRight = {screenPos.x + (screenSize.x / 2.f),
	                          screenPos.y - (screenSize.y / 2.f)};

	math::vec2 bottomLeft = {screenPos.x - (screenSize.x / 2.f),
	                         screenPos.y - (screenSize.y / 2.f)};

	math::vec2 topLeft = {screenPos.x - (screenSize.x / 2.f),
	                      screenPos.y + (screenSize.y / 2.f)};

	// title
	if (phx::ENUMhasFlag(flags, Flags::WITH_TITLE) ||
	    phx::ENUMhasFlag(flags, Flags::COLLAPSIBLE))
	{
		// gui font size, needs to be fixed at some point.
		const float fontSize = 16; // (pixels)

		// we want padding.
		const float padding = 10; // (pixels)

		const math::vec2 barSize = {screenSize.x,
		                            ((fontSize + padding) / windowSize.y)};

		const math::vec2 centerOfBar = {
		    screenPos.x, screenPos.y + (screenSize.y / 2.f) +
		                     ((padding + (fontSize / 2.f)) / windowSize.y)};

		// main verts for the menu bar.
		math::vec2 barTopRight    = {centerOfBar.x + (barSize.x / 2.f),
                                  centerOfBar.y + (barSize.y / 2.f)};
		math::vec2 barBottomRight = topRight;
		math::vec2 barBottomLeft  = topLeft;
		math::vec2 barTopLeft     = {centerOfBar.x - (barSize.x / 2.f),
                                 centerOfBar.y + (barSize.y / 2.f)};

		if (phx::ENUMhasFlag(flags, Flags::COLLAPSIBLE))
		{
			// triangle for the menu bar, pointing downwards because it's open
			// right now, not collapsed.
			math::vec2 triangleTopLeft = {
			    barTopRight.x -
			        (static_cast<float>(padding + fontSize) / windowSize.y),
			    barTopRight.y - (static_cast<float>(padding) / windowSize.y)};

			math::vec2 triangleTopRight = {
			    barTopRight.x - (static_cast<float>(padding) / windowSize.y),
			    barTopRight.y - (static_cast<float>(padding) / windowSize.y)};

			math::vec2 triangleBottomMiddle = {
			    barTopRight.x -
			        (static_cast<float>(padding + (fontSize / 2.f)) /
			         windowSize.y),
			    barTopRight.y -
			        (static_cast<float>(padding + fontSize) / windowSize.y)};

			// make sure this is pushed first, since if we collapse, we can just
			// turn this more easily.
			m_vertices.push_back({triangleTopLeft, {1, 1, 1}, 1, {}});
			m_vertices.push_back({triangleTopRight, {1, 1, 1}, 1, {}});
			m_vertices.push_back({triangleBottomMiddle, {1, 1, 1}, 1, {}});
		}

		if (phx::ENUMhasFlag(flags, Flags::WITH_TITLE))
		{
			// push_back text vertices.
		}

		// 128, 128, 128 is grey.
		// push 2 triangles for the bar.
		m_vertices.push_back({barTopRight, {0.5, 0.5, 0.5}, 1, {}});
		m_vertices.push_back({barBottomRight, {0.5, 0.5, 0.5}, 1, {}});
		m_vertices.push_back({barTopLeft, {0.5, 0.5, 0.5}, 1, {}});
		m_vertices.push_back({barBottomRight, {0.5, 0.5, 0.5}, 1, {}});
		m_vertices.push_back({barBottomLeft, {0.5, 0.5, 0.5}, 1, {}});
		m_vertices.push_back({barTopLeft, {0.5, 0.5, 0.5}, 1, {}});
	}

	m_numVertsInContextBar = m_vertices.size();

	// push vertices for main box. 90, 90, 90 is a darker grey so we can see the
	// distinction.
	m_vertices.push_back({topRight, {0.35, 0.35, 0.35}, 1, {}});
	m_vertices.push_back({bottomRight, {0.35, 0.35, 0.35}, 1, {}});
	m_vertices.push_back({topLeft, {0.35, 0.35, 0.35}, 1, {}});
	m_vertices.push_back({bottomRight, {0.35, 0.35, 0.35}, 1, {}});
	m_vertices.push_back({bottomLeft, {0.35, 0.35, 0.35}, 1, {}});
	m_vertices.push_back({topLeft, {0.35, 0.35, 0.35}, 1, {}});

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
	             m_vertices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, vert)));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, color)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, uv)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	m_shaderPipeline.prepare("Assets/GUIShader.vert", "Assets/GUIShader.frag",
	                         IComponent::getBufferLayout());

	// initialize a texture with nothing to prevent a crash, but a texture can't
	// just be non-existant, there has to be something.
	std::vector<float> emptyData(10 * 10 * 4, 1);
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10, 10, 0, GL_RGBA, GL_FLOAT,
	             emptyData.data());
}

Container::~Container()
{
	glDeleteBuffers(1, &m_buffer);
	glDeleteTextures(1, &m_texture);
	glDeleteVertexArrays(1, &m_vao);
}

phx::math::vec2 Container::getPosition() const { return m_position; }
phx::math::vec2 Container::getSize() const { return m_size; }

void Container::onEvent(events::Event e) {}

void Container::tick(float dt)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	m_shaderPipeline.activate();
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
}
