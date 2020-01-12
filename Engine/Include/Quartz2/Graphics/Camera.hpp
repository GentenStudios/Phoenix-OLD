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

#include <Quartz2/Math/Math.hpp>
#include <Quartz2/Graphics/Window.hpp>
#include <Quartz2/Settings.hpp>

#include <SDL.h>

#include <functional>
#include <vector>

namespace q2
{
	namespace gfx
	{
		/**
		 * @brief Base Camera class following basic FPS Cam principles.
		 *
		 * The camera functions as a basic tool for moving around in a 3D
		 * rendered world. This uses the "tick" function on every iteration of
		 * the main game loop to determine movement from the user.
		 */
		class FPSCamera
		{
		public:
			explicit FPSCamera(Window* window);

			math::vec3 getPosition() const;
			math::vec3 getDirection() const;
			void setProjection(const math::mat4& projection);
			math::mat4 getProjection() const;
			math::mat4 calculateViewMatrix() const;

			void tick(float dt);

			void enable(bool enabled);
			bool isEnabled() const { return m_enabled; }

			void onWindowResize(events::Event e);

		private:
			Window* m_window;

			math::mat4 m_projection;

			math::vec3 m_rotation;
			math::vec3 m_position;
			math::vec3 m_up;
			math::vec3 m_direction;

			math::vec2 m_windowCentre;

			bool m_enabled = true;

			Setting* m_settingSensitivity;
		};
	} // namespace gfx
} // namespace q2
