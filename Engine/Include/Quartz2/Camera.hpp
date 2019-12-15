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

#include <Quartz2/Matrix4x4.hpp>
#include <Quartz2/Vec3.hpp>
#include <Quartz2/Settings.hpp>

#include <SDL.h>

#include <cmath>

namespace q2
{
	/**
	 * @brief Base Camera class following basic FPS Cam principles.
	 *
	 * The camera functions as a basic tool for moving around in a 3D
	 * rendered world. This uses the "tick" function on every iteration of
	 * the main game loop to determine movement from the user.
	 */
	class Camera
	{
	public:
		Camera();

		/**
		 * @brief Gets the position of the camera, in world space
		 * @return A 3 component vector containing the position of the
		 * camera.
		 */
		Vec3 getPosition() const;

		/**
		 * @brief Gets the direction of the camera, in world space
		 * @return A 3 component vector containing the direction of the
		 * camera.
		 */
		Vec3 getDirection() const;

		/**
		 * @brief Calculates the view matrix for use in the graphics
		 * shaders.
		 * @return A 4x4 Matrix containing the required information to send
		 * into shaders.
		 */
		Mat4 calculateViewMatrix() const;

		/**
		 * @brief Updates things like position and direction dependent on
		 * whether the mouse has moved, or a key has been pressed.
		 *
		 * @param dt The Delta Time, the time between each frame, so the
		 * player moves at the same speed regardless of if they're playing
		 * at 100fps or 10fps.
		 */
		void tick(float dt, SDL_Window* window);

		/**
		 * @brief Sets the camera's state of being enabled or not being
		 * enabled.
		 * @param enabled A boolean stating whether the camera should be set
		 * to enabled or not.
		 */
		void enable(bool enabled);

		/**
		 * @brief Checks whether the camera is enabled or not.
		 * @return True if the camera is enabled, false if not.
		 */
		bool isEnabled() const { return m_enabled; }

	private:
		/// @brief The rotation of the camera.
		Vec3 m_rotation;

		/// @brief The position of the camera.
		Vec3 m_position;

		/// @brief The UP vector of the camera.
		Vec3 m_up;

		/// @brief The direction of the camera.
		Vec3 m_direction;

		/// @brief The current status of the camera, whether it's enabled or
		/// not.
		bool m_enabled = true;

		Setting m_settingSensitivity;
	};
} // namespace q2