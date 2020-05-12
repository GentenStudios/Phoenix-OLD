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

Container::Container(const std::string& name, math::vec2 position,
                     math::vec2 size, math::vec3 color, float alpha,
                     gfx::Window* window, Mode mode, Flags flags)
    : m_window(window)
{
	// coordinate system:
	// 50, 50 is the CENTER of the screen.
	// 00, 00 is bottom left.
	//
	// in OpenGL,
	// 0.0, 0.0 is the center of the screen.
	// -1, -1 is the bottom left of the screen.

	if (mode == Mode::RELATIVE)
	{
		// mode relative means we use the 50,50 is center.
		m_position = position / 100.f;
		m_size     = size / 100.f;
	}
	else if (mode == Mode::STATIC)
	{
		// mode static means we use pixels as coordinates.
		// we divide by the window size to get the relative position within the
		// screen, since OpenGL does not work with pixels, but rather
		// homogeneous coordinates.
		m_position = position / m_window->getSize();
		m_size     = size / m_window->getSize();
	}

	m_shaderPipeline.prepare("Assets/GUIShader.vert", "Assets/GUIShader.frag",
	                         IComponent::getBufferLayout());

	m_mainBox = new Rectangle(this, {50, 50}, {100, 100}, color, alpha);

	if (phx::ENUMhasFlag(flags, Flags::WITH_TITLE) ||
	    phx::ENUMhasFlag(flags, Flags::COLLAPSIBLE))
	{
		const math::vec2 fontSize {8.f}; // px
		const math::vec2 padding {3.f};   // px;

		const math::vec2 windowSize       = m_window->getSize();
		const math::vec2 relativeFontSize = (fontSize / windowSize) * size;
		const math::vec2 relativePadding  = (padding / windowSize) * size;

		m_collapseBox = new Rectangle(this, {50.f, 100.f + ((relativePadding.y + relativeFontSize.y))},
		    {100.f, 10},
		                              {0, 255, 0}, alpha);

		// add triangle if collapsible.
		// add text if with title.
	}

	// initialize a texture with nothing to prevent a
	// crash, but a texture can't just be non-existent,
	// there has to be something.
	std::vector<float> emptyData(10 * 10 * 4, 1);
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10, 10, 0, GL_RGBA, GL_FLOAT,
	             emptyData.data());
}

Container::~Container()
{
	delete m_mainBox;
	delete m_collapseBox;
}

void Container::attachComponent(IComponent* component)
{
	m_components.push_back(component);
}

void Container::detachComponent(IComponent* component)
{
	auto it = std::find(m_components.begin(), m_components.end(), component);
	if (it == m_components.end())
	{
		return;
	}

	m_components.erase(it);
}

phx::math::vec2 Container::getPosition(Mode mode) const
{
	if (mode == Mode::RELATIVE)
	{
		return m_position;
	}

	return m_position * static_cast<math::vec2>(m_window->getSize());
}

phx::math::vec2 Container::getSize(Mode mode) const
{
	if (mode == Mode::RELATIVE)
	{
		return m_size;
	}

	return m_size * static_cast<math::vec2>(m_window->getSize());
}

phx::gfx::Window* Container::getWindow() const { return m_window; }

void Container::onEvent(events::Event e) {}

void Container::tick(float dt)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	m_shaderPipeline.activate();

	m_mainBox->tick(dt);
	m_collapseBox->tick(dt);
	
	//for (auto it = m_components.rbegin(); it != m_components.rend(); ++it)
	//{
	//	(*it)->tick(dt);
	//}
}
