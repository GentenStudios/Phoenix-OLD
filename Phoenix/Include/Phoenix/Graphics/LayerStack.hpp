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

#include <Phoenix/Graphics/Layer.hpp>
#include <Phoenix/Events/IEventListener.hpp>
#include <Phoenix/Graphics/Window.hpp>

#include <vector>

namespace phx
{
	namespace gfx
	{
		class LayerStack : public events::IEventListener
		{
			using Storage = std::vector<Layer*>;

		public:
			LayerStack(gfx::Window* window) : m_window(window) {}
			~LayerStack();

			void pushLayer(Layer* layer);
			void popLayer(Layer* layer);
			void pushOverlay(Layer* overlay);
			void popOverlay(Layer* overlay);

			void onEvent(events::Event e) override;
			void tick(float dt);

			// generic accessor methods.
			std::size_t size() const { return m_layers.size(); }
			bool empty() const { return m_layers.empty(); }
			
			// iterator methods
			Storage::iterator         begin() { return m_layers.begin(); }
			Storage::iterator         end() { return m_layers.end(); }
			Storage::reverse_iterator rbegin() { return m_layers.rbegin(); }
			Storage::reverse_iterator rend() { return m_layers.rend(); }

			Storage::const_iterator begin() const { return m_layers.begin(); }
			Storage::const_iterator end() const { return m_layers.end(); }
			Storage::const_reverse_iterator rbegin() const
			{
				return m_layers.rbegin();
			}
			Storage::const_reverse_iterator rend() const
			{
				return m_layers.rend();
			}

			Storage::reference       front() { return m_layers.front(); }
			Storage::const_reference front() const { return m_layers.front(); }

			Storage::reference       back() { return m_layers.back(); }
			Storage::const_reference back() const { return m_layers.back(); }
			// end iterator methods

		private:
			Storage m_layers;
			unsigned int m_currentInsert = 0;

			gfx::Window* m_window;
		};
	} // namespace gfx
} // namespace phx
