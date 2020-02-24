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

#include <Client/Events/IEventListener.hpp>
#include <Client/Graphics/Layer.hpp>
#include <Client/Graphics/Window.hpp>

#include <vector>

namespace phx
{
	namespace gfx
	{
		/**
		 * @brief A stack of layers automagically managed internally.
		 *
		 * This class provides the ability to render multiple things at once
		 * with more ease and modularity. A functional point is to be able to
		 * push and pop new layers as required, as well as allowing automatic
		 * removal of layers that mark themselves as finished and request
		 * removal.
		 *
		 * @paragraph Usage
		 * Usage for this class can get slightly complicated in terms of memory
		 * management. When you push a layer, you are passing ownership of said
		 * layer TO the LayerStack. It will remember to delete the dynamically
		 * allocated memory if automatically popped (through
		 * Layer::signalRemoval) or on destruction of the LayerStack.
		 *
		 * If you manually pop a layer, you RECLAIM ownership, it proves that
		 * you still have a functional copy to the pointer of the layer, so you
		 * must manage it's memory - it's best to not delete it until you KNOW
		 * you don't need it, allocating graphical resources can be expensive
		 * depending on the use.
		 *
		 * @code
		 * auto window = new Window(...);
		 * auto layerStack = new LayerStack(window);
		 *
		 * class EventHandler : public IEventListener
		 * {
		 * public:
		 *     void onEvent(Event e)
		 *     {
		 *         switch (e.type)
		 *         {
		 *         case EventType::KEY_PRESSED:
		 *		       ... (set e.handled to true if you actually handle a key
		 *                  event)
		 *
		 *             if (!e.handled)
		 *                 layerStack->onEvent(e);
		 *
		 *		       break;
		 *		   case EventType::LAYER_DESTROYED
		 *		       if (e.layer == "SplashScreen")
		 *		       {
		 *                 GameLayer* game = new GameLayer();
		 *                 layerStack.pushLayer(game); // you've passed
		 *                                             // ownership of the layer
		 *                                             // to the stack.
		 *		           e.handled = true;
		 *		       }
		 *
		 *		       if (!e.handled)
		 *		           layerStack->onEvent(e);
		 *
		 *		       break;
		 *         default:
		 *             layerStack->onEvent(e);
		 *             break;
		 *         }
		 *     }
		 * };
		 *
		 * class SplashScreen
		 * {
		 * public:
		 *     void onAttach() { ... }
		 *     void onDetach() { ... }
		 *
		 *     void tick() { ... if (m_finished) { signalRemoval(); } ... }
		 * };
		 *
		 * auto SplashScreen = new SplashScreen();
		 *
		 * while (window->isRunning())
		 * {
		 *     float dt = *do delta time calculations*;
		 *     window->startFrame();
		 *
		 *     layerStack->tick();
		 *
		 *     window->endFrame();
		 * }
		 * @endcode
		 */
		class LayerStack : public events::IEventListener
		{
			// scroll down, you'll see why ;)
			using Storage = std::vector<Layer*>;

		public:
			/**
			 * @brief Constructs a LayerStack bound to a window.
			 * @param window The window this LayerStack is attached to.
			 */
			explicit LayerStack(Window* window);
			~LayerStack();

			/**
			 * @brief Pushes another layer onto the stack.
			 * @param layer The layer to push onto the stack.
			 *
			 * This method assumes control over the Layer's memory lifetime. You
			 * are **HANDING OWNERSHIP** of the allocated memory to the layer
			 * stack.
			 */
			void pushLayer(Layer* layer);

			/**
			 * @brief Pops a layer from the stack.
			 * @param layer The layer to remove from the stack.
			 *
			 * This method returns memory ownership of the layer to the parent
			 * class. This is to prevent unintentional deletions when manually
			 * popping a layer, just in case it is used again so graphics
			 * resources are not unnecessarily re-allocated.
			 */
			void popLayer(Layer* layer);

			/**
			 * @brief Pushes another overlay onto the stack.
			 * @param overlay The overlay to push onto the stack.
			 *
			 * This method assumes control over the Overlay's memory lifetime.
			 * You are **HANDING OWNERSHIP** of the allocated memory to the
			 * layer stack.
			 */
			void pushOverlay(Layer* overlay);

			/**
			 * @brief Pops an overlay from the stack.
			 * @param overlay The overlay to remove from the stack.
			 *
			 * This method returns memory ownership of the overlay to the parent
			 * class. This is to prevent unintentional deletions when manually
			 * popping an overlay, just in case it is used again so graphics
			 * resources are not unnecessarily re-allocated.
			 */
			void popOverlay(Layer* overlay);

			/**
			 * @brief Processes the event provided.
			 * @param e The event to process.
			 *
			 * This is a virtual function inherited from events::IEventListener,
			 * so just check that out for more information.
			 */
			void onEvent(events::Event e) override;

			/**
			 * @brief Ticks all layers once, along with internal mechanisms.
			 * @param dt The delta time.
			 *
			 * The delta time is the time it took for the last frame to finish
			 * completely processing.
			 */
			void tick(float dt);

			// generic accessor methods.
			std::size_t size() const;
			bool        empty() const;

			// iterator methods
			Storage::iterator       begin();
			Storage::iterator       end();
			Storage::const_iterator begin() const;
			Storage::const_iterator end() const;

			Storage::reverse_iterator       rbegin();
			Storage::reverse_iterator       rend();
			Storage::const_reverse_iterator rbegin() const;
			Storage::const_reverse_iterator rend() const;

			Storage::reference       front();
			Storage::reference       back();
			Storage::const_reference front() const;
			Storage::const_reference back() const;

		private:
			Storage      m_layers;
			unsigned int m_currentInsert = 0;

			Window* m_window;
		};
	} // namespace gfx
} // namespace phx
