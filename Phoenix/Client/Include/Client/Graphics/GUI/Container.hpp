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

#include <Client/Events/IEventListener.hpp>
#include <Client/Graphics/GUI/IComponent.hpp>
#include <Client/Graphics/Window.hpp>
#include <Client/Graphics/ShaderPipeline.hpp>

#include <Common/EnumTools.hpp>
#include <Common/Logger.hpp>
#include <Common/Math/Math.hpp>

#include <algorithm>
#include <string>
#include <vector>
#include <type_traits>

namespace phx::gui
{
	class Container : public events::IEventListener
	{
	public:
		enum class Flags
		{
			WITH_TITLE,
			COLLAPSIBLE
		};
	public:
		Container(const std::string& name, math::vec2 pos, math::vec2 size,
		          gfx::Window* window);
		~Container();

		template <typename T, typename U = std::enable_if_t<
		                          std::is_base_of<IComponent, T>::value>>
		T* create()
		{
			IComponent* component = new T();
			m_components.push_back(component);

			component->container = this;

			return static_cast<T*>(component);
		}

		template <typename T, typename U = std::enable_if_t<
		                          std::is_base_of<IComponent, T>::value>>
		void destroy(T* component)
		{
			const auto it = std::find(m_components.begin(), m_components.end(),
			                    static_cast<IComponent*>(component));
			if (it == m_components.end())
			{
				return;
			}

			m_components.erase(it);
			delete component;
		}

		void onEvent(events::Event e) override;
		void tick(float dt);

	private:
		unsigned int m_buffer;
		unsigned int m_vao;

		// used if we collapse the window.
		std::size_t m_numVertsInContextBar = 0;

		gfx::ShaderPipeline m_shaderPipeline;
		
		std::vector<IComponent*> m_components;
	};
} // namespace phx::gui

ENABLE_BITWISE_OPERATORS(phx::gui::Container::Flags);
