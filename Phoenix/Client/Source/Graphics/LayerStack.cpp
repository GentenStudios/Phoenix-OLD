// Copyright 2019-20 Genten Studios
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

#include <Client/Events/Event.hpp>
#include <Client/Graphics/LayerStack.hpp>

#include <algorithm>

using namespace phx::gfx;
using namespace phx;

LayerStack::LayerStack(gfx::Window* window) : m_window(window) {}

std::size_t LayerStack::size() const { return m_layers.size(); }

bool LayerStack::empty() const { return m_layers.empty(); }

std::vector<Layer*>::iterator LayerStack::begin() { return m_layers.begin(); }

std::vector<Layer*>::iterator LayerStack::end() { return m_layers.end(); }

std::vector<Layer*>::reverse_iterator LayerStack::rbegin()
{
	return m_layers.rbegin();
}

std::vector<Layer*>::reverse_iterator LayerStack::rend()
{
	return m_layers.rend();
}

std::vector<Layer*>::const_iterator LayerStack::begin() const
{
	return m_layers.begin();
}

std::vector<Layer*>::const_iterator LayerStack::end() const
{
	return m_layers.end();
}

std::vector<Layer*>::const_reverse_iterator LayerStack::rbegin() const
{
	return m_layers.rbegin();
}

std::vector<Layer*>::const_reverse_iterator LayerStack::rend() const
{
	return m_layers.rend();
}

std::vector<Layer*>::reference LayerStack::front() { return m_layers.front(); }

std::vector<Layer*>::const_reference LayerStack::front() const
{
	return m_layers.front();
}

std::vector<Layer*>::reference LayerStack::back() { return m_layers.back(); }

std::vector<Layer*>::const_reference LayerStack::back() const
{
	return m_layers.back();
}

LayerStack::~LayerStack()
{
	for (Layer* layer : m_layers)
	{
		layer->onDetach();
		delete layer;
	}
}

void LayerStack::pushLayer(Layer* layer)
{
	m_layers.emplace(m_layers.begin() + m_currentInsert, layer);
	++m_currentInsert;
	layer->onAttach();

	events::Event e;
	e.type  = events::EventType::LAYER_PUSHED;
	e.layer = layer->getName().c_str();
	m_window->dispatchCustomEvent(e);
}

void LayerStack::popLayer(Layer* layer)
{
	const auto it = std::find(m_layers.begin(), m_layers.end(), layer);
	if (it != m_layers.end())
	{
		layer->onDetach();
		--m_currentInsert;
		m_layers.erase(it);

		events::Event e;
		e.type  = events::EventType::LAYER_DESTROYED;
		e.layer = layer->getName().c_str();
		m_window->dispatchCustomEvent(e);
	}
}

void LayerStack::pushOverlay(Layer* overlay)
{
	m_layers.emplace_back(overlay);
	overlay->onAttach();

	events::Event e;
	e.type  = events::EventType::LAYER_PUSHED;
	e.layer = overlay->getName().c_str();
	m_window->dispatchCustomEvent(e);
}

void LayerStack::popOverlay(Layer* overlay)
{
	const auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
	if (it != m_layers.end())
	{
		overlay->onDetach();
		m_layers.erase(it);

		events::Event e;
		e.type  = events::EventType::LAYER_DESTROYED;
		e.layer = overlay->getName().c_str();
		m_window->dispatchCustomEvent(e);
	}
}

void LayerStack::onEvent(events::Event e)
{
	for (std::size_t i = m_layers.size(); i > 0; --i)
	{
		// arrays start at 0, hence the -1;
		m_layers[i - 1]->onEvent(e);
		if (e.handled)
			break;
	}
}

void LayerStack::tick(float dt)
{
	if (m_layers.empty())
		m_window->close();

	auto it = m_layers.begin();
	while (it != m_layers.end())
	{
		if ((*it)->requiresRemoval())
		{
			if ((*it)->isOverlay())
			{
				std::string layerName = (*it)->getName();

				(*it)->onDetach();
				delete *it;
				const std::size_t pos =
				    std::distance(m_layers.begin(), it) -
				    m_currentInsert; // taking away m_currentInsert because we
				                     // only want the position relative to the
				                     // overlays; remember: overlays are stored
				                     // after all the layers to make sure they
				                     // are rendered "on top", and to do so,
				                     // they are played in the second half of
				                     // the vector, all overlays are pushed to
				                     // the back, but new layers are pushed to
				                     // where the half-way line is - just before
				                     // the overlays. If this doesn't make sense
				                     // and for some reason this functionality
				                     // needs updating, contact @beeperdeeper089
				                     // (19/02/2020).
				it = m_layers.erase(it);

				events::Event e;
				e.type  = events::EventType::LAYER_DESTROYED;
				e.layer = layerName.c_str();
				m_window->dispatchCustomEvent(e);

				// the above event could have made another element be pushed to
				// the array, so all we're doing here is recalculating the
				// iterator position inside the vector in case something was
				// added due to the handling of above event. Remember:
				// adding/removing something from a vector can invalidate the
				// iterators - so this is to make sure.
				//
				// For the overlay's in specific, since they're stored in the
				// 2nd half of the array, m_currentInsert IS the point at which
				// the Layers end and Overlays start. We're taking away
				// m_currentInsert above in the std::distance calculation
				// because we want the position of the overlay in the 2nd half
				// of the array, since the size of the first half could change.
				// If this doesn't make sense and for some reason this
				// functionality needs updating, contact @beeperdeeper089
				// (19/02/2020).
				it = m_layers.begin() + m_currentInsert + pos;
			}
			else
			{
				std::string layerName = (*it)->getName();

				(*it)->onDetach();
				delete *it;
				--m_currentInsert;
				const std::size_t pos = std::distance(m_layers.begin(), it);
				it                    = m_layers.erase(it);

				events::Event e;
				e.type  = events::EventType::LAYER_DESTROYED;
				e.layer = layerName.c_str();
				m_window->dispatchCustomEvent(e);

				// as I've said above, but since this is in the first half of
				// the array, before the m_currentInsert actually matters, we
				// don't need to deal with that here.
				it = m_layers.begin() + pos;
			}
		}
		else
		{
			(*it)->tick(dt);
			++it;
		}
	}
}

