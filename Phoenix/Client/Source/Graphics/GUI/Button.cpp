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

#include <Client/Graphics/GUI/Button.hpp>

#include <Common/Logger.hpp>

#include <iostream>

using namespace phx::gui;

Button::Button(Container* container, math::vec2 pos, math::vec2 size,
               math::vec3 color, float alpha)
    : IComponent(container), m_pos(pos), m_size(size),
      m_rectangle(container, pos, size, color, alpha, true)
{
	// we are putting rectangle in the initializer list since we don't really
	// want to heap allocate it for memory locality and because there is no
	// default constructor for it.

	container->attachComponent(this);
}

Button::~Button() { container->detachComponent(this); }

void Button::setCallback(const Callback& callback) { m_callback = callback; }

phx::math::vec2 Button::getPosition() const { return m_pos; }

void Button::setPosition(const math::vec2& position)
{
	m_pos = position;
	m_rectangle.setPosition(position);
}

phx::math::vec2 Button::getSize() const { return m_size; }

void Button::setSize(const math::vec2& size)
{
	m_size = size;
	m_rectangle.setSize(size);
}

void Button::onEvent(events::Event& event)
{
	if (event.type == events::EventType::MOUSE_BUTTON_PRESSED)
	{
		if (m_rectangle.isPointInObject({event.mouse.x, event.mouse.y}))
		{
			if (m_callback)
			{
				m_callback(event);
			}

			event.handled = true;
		}
	}
}

void Button::tick(float dt) { m_rectangle.tick(dt); }
