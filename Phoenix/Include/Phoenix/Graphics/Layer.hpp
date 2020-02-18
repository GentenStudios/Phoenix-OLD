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

#include <string>

#include <Phoenix/Events/Event.hpp>

namespace phx
{
	namespace gfx
	{
		// design of this class heavily inspired by TheCherno.
		class Layer
		{
		public:
			Layer(const std::string& name) : m_name(name) {}
			virtual ~Layer() = default;

			virtual void onAttach()               = 0;
			virtual void onDetach()               = 0;
			virtual void onEvent(events::Event e) = 0;

			virtual void tick(float dt) = 0;

			const std::string& getName() const { return m_name; };

			void signalRemoval() { m_requiresRemoval = true; };
			bool requiresRemoval() const { return m_requiresRemoval; }

			virtual bool isOverlay() { return false; };

		protected:
			std::string m_name;
			bool        m_requiresRemoval = false;
		};

		class Overlay : public Layer
		{
		public:
			bool isOverlay() { return true; }
		};

	} // namespace gfx
} // namespace phx