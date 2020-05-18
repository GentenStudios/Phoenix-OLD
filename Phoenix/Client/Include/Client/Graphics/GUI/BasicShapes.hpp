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

#pragma once

#include <Client/Graphics/GUI/IComponent.hpp>

#include <Common/Math/Math.hpp>

namespace phx::gui
{
	/**
	 * @brief Base class for all primitive shapes to inherit from.
	 */
	struct Shape
	{
		// to check is a point is in the object, for example if you clicked in
		// it.
		virtual bool isPointInObject(const math::vec2& pos) = 0;
	};

	/**
	 * @brief Produces a rendered rectangle for the GUI.
	 */
	class Rectangle : public Shape, IComponent
	{
	public:
		/**
		 * @brief Specifies each corner of the rectangle.
		 */
		enum class Corner
		{
			TOP_RIGHT,
			BOTTOM_RIGHT,
			BOTTOM_LEFT,
			TOP_LEFT
		};

	public:
		/**
		 * @brief Creates a renderable rectangle for the GUI.
		 * @param container The container to tie the component to.
		 * @param pos The position of the component within the container.
		 * @param size The size of the component within the container.
		 * @param color The color of the component.
		 * @param alpha The alpha (translucency) of the component.
		 * @param hasParent If the rectangle is an object on it's own or is
		 * surrounded in another object.
		 *
		 * hasParent should be true if surrounded - like in a button. The button
		 * attaches itself to the container (Container::attachComponent) so the
		 * button doesn't need to do it.
		 */
		Rectangle(Container* container, math::vec2 pos, math::vec2 size,
		          math::vec3 color, float alpha, bool hasParent = false);

		virtual ~Rectangle();

		/**
		 * @brief Gets the vertex data for a specific corner from the rectangle.
		 * @param corner The corner to retrieve from the buffer.
		 * @return The vertex data for the respective corner.
		 */
		Vertex getCorner(Corner corner);

		/**
		 * @brief Sets a corner to specific vertex data.
		 * @param corner The corner to set.
		 * @param vertex The vertex to set the corner to.
		 */
		void setCorner(Corner corner, const Vertex& vertex);

		/**
		 * @brief Checks whether a point is within the rectangle.
		 * @param pos The position to check for intersection.
		 * @return Whether or not the position is within the rectangle.
		 */
		bool isPointInObject(const math::vec2& pos) override;

		/**
		 * @brief Gets the position of the rectangle (relative to container).
		 * @return The position of the rectangle.
		 */
		math::vec2 getPosition() const override;

		/**
		 * @brief Sets a new position to the rectangle (relative to container).
		 * @param position The new position to set the rectangle to.
		 */
		void setPosition(const math::vec2& position) override;

		/**
		 * @brief Gets the size of the rectangle (relative to container).
		 * @return The size of the rectangle.
		 */
		math::vec2 getSize() const override;

		/**
		 * @brief Sets the new size of the rectangle (relative to container).
		 * @param size The new size of the rectangle.
		 */
		void setSize(const math::vec2& size) override;

		/**
		 * @brief Processes an event.
		 * @param event The event to process.
		 */
		void onEvent(events::Event& event) override;

		/**
		 * @brief Ticks the rectangle (re-renders it).
		 * @param dt The time passed since the last frame.
		 */
		void tick(float dt) override;

	private:
		std::vector<Vertex> m_vertices;

		math::vec2 m_pos;
		math::vec2 m_size;

		unsigned int m_vao    = 0;
		unsigned int m_buffer = 0;
	};
} // namespace phx::gui
