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
#include <Client/Graphics/GUI/BasicShapes.hpp>
#include <Client/Graphics/Window.hpp>
#include <Client/Graphics/ShaderPipeline.hpp>

#include <Common/EnumTools.hpp>
#include <Common/Math/Math.hpp>

#include <algorithm>
#include <string>
#include <vector>
#include <type_traits>

namespace phx::gui
{
	/**
	 * @brief The class surrounding all GUI components.
	 *
	 * This class is a way of containing GUI elements and placing them
	 * relatively throughout the size of the container.
	 *
	 * @paragraph Usage
	 *
	 * When creating the container, you must provide a window title (text
	 * currently is not rendered), relative position and relative size within the
	 * window.
	 *
	 * The scale of the "relative" measurement is between 0 to 100, with 50
	 * being the center. When creating a container and/or component, using 50,
	 * 50 for position would indicate the center of the screen/container and
	 * using 50, 50 for size would indicate half the screen/container in size.
	 * 
	 * @code
	 * gui::Container container = gui::Container("", { 50, 50 }, { 50, 50 },
	 *					{ 255, 255, 255 }, 1.f, window);
	 *
	 * gui::Button button = gui::Button(container, { 50, 50 }, { 50, 50 },
	 *					{ 255, 255, 255 }, 1.f);
	 *
	 *	while (window->isRunning())
	 *	{
	 *		container.tick();
	 *	}
	 * @endcode
	 */
	class Container : public events::IEventListener
	{
		template <class Derived, class Base>
		using IsDerivedFrom = std::enable_if_t<std::is_base_of_v<Base, Derived>>;

	public:
		/**
		 * @brief Flags for the style of window.
		 *
		 * Currently unimplemented, will document once implementation is
		 * completed.
		 */
		enum class Flags
		{
			WITH_TITLE = 1,
			COLLAPSIBLE = 1 << 1,
			CLEAN = 1 << 2,
		};

	public:
		/**
		 * @brief Constructs a container of a specific position and size.
		 * @param name The name of the container.
		 * @param position The position of the container (between 0 and 100).
		 * @param size The size of the container (between 0 and 100).
		 * @param color The color of the container.
		 * @param alpha The alpha (translucency) of the container.
		 * @param window The window it is attached to.
		 * @param flags The style of the container.
		 */
		Container(const std::string& name, math::vec2 position, math::vec2 size,
		          math::vec3 color, float alpha, gfx::Window* window,
		          Flags flags = Flags::CLEAN);
		~Container();

		/**
		 * @brief Detaches a component to the GUI.
		 * @param component The component to attach.
		 */
		void attachComponent(IComponent* component);

		/**
		 * @brief Detaches a component from the container.
		 * @param component The component to detach.
		 */
		void detachComponent(IComponent* component);

		/**
		 * @brief Gets the position of the container.
		 * @return The position of the container.
		 */
		const math::vec2& getPosition() const;

		/**
		 * @brief Gets the size of the container.
		 * @return The size of the container.
		 */
		const math::vec2& getSize() const;

		/**
		 * @brief Gets the window the container is tied to.
		 * @return The window object the container is tied to.
		 */
		gfx::Window* getWindow() const;

		/**
		 * @brief Processes an event.
		 * @param e The event to process.
		 */
		void onEvent(events::Event e) override;

		/**
		 * @brief Ticks the GUI, re-renders and updates any elements needing it.
		 * @param dt The change in time since the previous frame.
		 */
		void tick(float dt);

	private:
		bool m_collapsed = false;
		bool m_collapsible = false;

		Rectangle* m_mainBox = nullptr;
		Rectangle* m_collapseBox = nullptr;

		// relative position & size.
		math::vec2 m_position;
		math::vec2 m_size;

		gfx::Window* m_window;
		
		// temporary
		unsigned int m_texture;
		gfx::ShaderPipeline m_shaderPipeline;

		std::vector<IComponent*> m_components;
	};
} // namespace phx::gui

ENABLE_BITWISE_OPERATORS(phx::gui::Container::Flags);
