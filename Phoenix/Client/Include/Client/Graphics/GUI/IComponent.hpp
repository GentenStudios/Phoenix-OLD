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

#include <Client/Events/Event.hpp>
#include <Client/Graphics/ShaderPipeline.hpp>

#include <Common/Math/Math.hpp>

namespace phx::gui
{
	class Container;

	/**
	 * @brief Data layout for each vertex in the GUI.
	 */
	struct Vertex
	{
		math::vec2 vert;
		math::vec3 color;
		float      alpha = 0.f;
		math::vec2 uv;
	};

	/**
	 * @brief The Interface for a GUI Component.
	 *
	 * The GUI is built up of containers and components. Containers contain
	 * components and each component is ticked in order to produce the layout
	 * required.
	 */
	struct IComponent
	{
		/**
		 * @brief Constructs a component, no extra functionality.
		 * @param container The container to set the internal pointer to.
		 */
		IComponent(Container* container) : container(container) {}

		virtual ~IComponent() = default;

		/**
		 * @brief Gets the layout of the buffer required by the shader.
		 * @return The layout of the data that the shader should expect.
		 */
		static std::vector<gfx::ShaderLayout> getBufferLayout()
		{
			return {{"a_Vertex", 0}, {"a_Color", 1}, {"a_UV", 2}};
		}

		virtual math::vec2 getPosition() const              = 0;
		virtual void       setPosition(const math::vec2& position) = 0;

		virtual math::vec2 getSize() const          = 0;
		virtual void       setSize(const math::vec2& size) = 0;

		virtual void onEvent(events::Event& event) = 0;
		virtual void tick(float dt)               = 0;

		bool       enabled   = true;
		Container* container = nullptr;
	};
} // namespace phx::gui
