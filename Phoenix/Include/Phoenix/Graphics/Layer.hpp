// Copyright 2019 Genten Studios
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

#include <Phoenix/Events/Event.hpp>

#include <string>

namespace phx
{
	namespace gfx
	{
		/**
		 * @brief A component of the game or client.
		 *
		 * The Layer and LayerStack principle works just like the idea of Layers
		 * in Photoshop. Every layer is rendered, but in order, the newest layer
		 * is rendered last to ensure it's "on top" (Depth Buffers can
		 * complicate this), and then overlays are rendered on top of all the
		 * layers.
		 *
		 * "Tick" is the update function, it is called every frame and should
		 * process updates to the system and/or anything it needs to render.
		 * There is no "render" method, so you must call your render functions
		 * in this function if there is something you need to render.
		 *
		 * @see LayerStack
		 */
		class Layer
		{
		public:
			/**
			 * @brief Constructs a layer with a specific name.
			 * @param name The name of the layer. (keep it short for SSO.)
			 *
			 * Please construct a layer using the ``new`` keyword. This is
			 * necessary since the layer stack may take ownership of the layer
			 * object once pushed, and ownership will only be returned on call
			 * of "pop*" on the LayerStack. If this is not dynamically-allocated
			 * and the LayerStack tries to call ``delete``, you're in a pickle
			 * and the program will segfault.
			 */
			explicit Layer(const std::string& name) : m_name(name) {}
			virtual ~Layer() = default;

			/**
			 * @brief Runs code when a layer is attached to the stack.
			 *
			 * This class provides you with the ability to define a function to
			 * run when "attached" to the LayerStack (aka, submitted to the
			 * LayerStack for use). This function should usually be used to
			 * initialise necessities, such as graphical resources and objects
			 * it needs created.
			 */
			virtual void onAttach() = 0;

			/**
			 * @brief Runs code when a layer is detached from the stack.
			 *
			 * This class also provides the ability to define a function to run
			 * when "detached" from the LayerStack. This function should usually
			 * be used to de-initialise graphics resources, however anything
			 * that is intensive to "delete" should be saved from the
			 * destructor, since ``onDetach`` does NOT guarantee end of life, a
			 * layer can be re-attached, for example like an escape menu, it
			 * would be pushed when requested, and popped once the user is done
			 * with it - but it can definitely be used again and we should not
			 * completely get rid of it for no reason.
			 */
			virtual void onDetach() = 0;

			/**
			 * @brief Processes an event.
			 * @param e The event to process.
			 *
			 * This method is just like the method in events::IEventListener. It
			 * is called with a reference to an event, so you can set
			 * ``e.handled`` to true if you actually handle the event. This is
			 * very important since the ``handled`` member of the Event object
			 * defines whether the LayerStack/Application should propagate the
			 * event further down the system.
			 */
			virtual void onEvent(events::Event& e) = 0;

			/**
			 * @brief "Ticks" the frame, run every frame as a way of updating.
			 * @param dt The delta time.
			 *
			 * This function is the update function. It is called every frame
			 * and should process updates to the system and/or anything it needs
			 * to render. There is no render method, so you must call your
			 * render functions in this method if there is something that
			 * actually needs to be rendered.
			 */
			virtual void tick(float dt) = 0;

			/**
			 * @brief Gets the name of the layer.
			 * @return The name of the layer.
			 */
			const std::string& getName() const { return m_name; };

			/**
			 * @brief Signals to the layer stack that this layer needs to be removed.
			 *
			 * As a tool of convenience, the LayerStack provides the ability for
			 * Layers to request being removed internally. This basically just
			 * sets a variable that the LayerStack then reads and processes the
			 * removal of this layer as required.
			 */
			void signalRemoval() { m_requiresRemoval = true; };

			// used by the LayerStack, but are self explanatory.
			bool requiresRemoval() const { return m_requiresRemoval; }
			bool isOverlay() const { return false; };

		protected:
			std::string m_name;
			bool        m_requiresRemoval = false;
		};

		/**
		 * @brief A overlaying component of the game or client.
		 *
		 * This is essentially the same as Layer, just a partial specialisation
		 * of it, in terms of inheritance (not the correct terminology since
		 * these aren't templates). It just overrides the isOverlay function to
		 * say that it is, which allows the LayerStack to process it
		 * differently.
		 *
		 * @see Layer
		 */
		class Overlay : public Layer
		{
		public:
			explicit Overlay(const std::string& name) : Layer(name) {}
			~Overlay() override = default;

			bool isOverlay() const { return true; }
		};

	} // namespace gfx
} // namespace phx