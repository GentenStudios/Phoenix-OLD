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
	// shape instead of component because we don't need onEvent on a normal
	// shape.
	class Container;
	struct Shape
	{
		virtual ~Shape() = 0;

		virtual void update(math::vec2 moved, math::vec2 scale) = 0;

		// to check is a point is in the object, for example if you clicked in
		// it.
		virtual bool isPointInObject(math::vec2 pos) = 0;

		Container* container;
	};

	class Rectangle : public Shape
	{
	public:
		enum class Corner
		{
			TOP_RIGHT,
			BOTTOM_RIGHT,
			BOTTOM_LEFT,
			TOP_LEFT
		};

	public:
		Rectangle();
		virtual ~Rectangle();

		void init(math::vec2 position, math::vec2 size);

		Vertex getCorner(Corner corner);
		void   setCorner(Corner corner, Vertex position);

		// this is relative.
		// if you move it left and down, position is NEGATIVE (pixelsMoved /
		// windowSize) if it shrinks, scale is between 0 and 1. if it grows,
		// scale is >1. - it's a scaling thing.
		void update(math::vec2 moved, math::vec2 scale);

		bool isPointInObject(math::vec2 pos);
		void tick(float dt);

	private:
		std::vector<Vertex> m_vertices;

		math::vec2 m_pos;
		math::vec2 m_size;

		unsigned int m_vao    = 0;
		unsigned int m_buffer = 0;
	};
} // namespace phx::gui
