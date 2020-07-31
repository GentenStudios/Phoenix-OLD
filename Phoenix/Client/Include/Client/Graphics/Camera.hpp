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

/**
 * @file Camera.hpp
 * @brief The main class for the FPS Camera design.
 *
 * @copyright Copyright (c) 2019-20 Genten Studios
 */

#pragma once

#include <Client/Graphics/Window.hpp>

#include <Client/InputMap.hpp>
#include <Client/Graphics/ChunkView.hpp>

#include <Common/Math/Math.hpp>
#include <Common/Settings.hpp>

#include <entt/entt.hpp>

#include <atomic>

namespace phx::gfx
{
	/**
	 * @brief Base Camera class following basic FPS Cam principles.
	 *
	 * The camera functions as a basic tool for moving around in a 3D
	 * rendered world. This uses the "tick" function on every iteration of
	 * the main game loop to determine movement from the user.
	 *
	 * The camera moves in the direction the player is facing, this will
	 * change as more of the game is developed.
	 *
	 * @paragraph Usage
	 * @code
	 * FPSCamera* camera = new Camera(window);
	 *
	 * // this is pure theoretical, this code would not work ever...
	 * if (windowFocusLost) camera->enable(false);
	 * if (windowFocusGained) camera->enable(true);
	 * if (escapeKeyPressed) camera-<enable(!camera->isEnabled());
	 *
	 * void mainGameLoop()
	 * {
	 *     camera->tick(timeToRenderLastFrame);
	 *
	 *     shader.setProjection(camera->getProjection());
	 *     shader.setViewMatrix(camera->calculateViewMatrix());
	 *
	 *     world.render();
	 * }
	 * @endcode
	 */
	class FPSCamera
	{
	public:
		/**
		 * @brief Constructs the FPS Camera.
		 * @param window The window object you want tied to the camera.
		 *
		 * The window object is required for requesting mouse position
		 * during the tick function. There is no workaround for this that I
		 * know of, however this is fine for the most part.
		 */
		explicit FPSCamera(Window* window, entt::registry* registry);

		/**
		 * @brief Sets the camera perspective projection.
		 * @param projection The actual projection to set the camera to.
		 *
		 * The projection matrix helps the "clipper" represent coordinates
		 * in a normalized space of -1 to 1. The matrix means the clipper
		 * can do its work without needing to know screen dimensions and the
		 * near/far plane.
		 *
		 * Reference:
		 * http://ogldev.atspace.co.uk/www/tutorial12/tutorial12.html
		 */
		void setProjection(const math::mat4& projection);

		/**
		 * @brief Gets the perspective projection of the camera.
		 * @return The projection matrix to pass to shaders or anything else
		 * that might need it (unlikely).
		 *
		 * The return of this function is usually passed to the shader - other
		 * uses are very limited.
		 */
		math::mat4 getProjection() const;

		/**
		 * @brief Calculates and returns the view matrix.
		 * @return The view matrix to pass to the shader.
		 *
		 * This view matrix, alongside the model and perspective projection
		 * matrix are the requirements for the shader, as they represent
		 * position, direction and what is wanted to be seen on screen.
		 *
		 * The view matrix will hold the position and the direction of the
		 * player, and through the MVP (Model View Projection) calculations
		 * happening in the shader, it will translate everything to screen
		 * space coordinates.
		 */
		math::mat4 calculateViewMatrix() const;

		/**
		 * @brief Updates values within the camera.
		 * @param dt The time it took to render the last frame.
		 *
		 * This function updates the camera by reading the mouse position
		 * and the keys being pressed at that point in time. It calculates
		 * the amount the mouse has moved from the centre of the screen and
		 * will warp the mouse TO THE CENTRE of the window. This is
		 * important as you MUST disable the camera through
		 * ``camera->enable(false)``, otherwise you will not be able to move
		 * the mouse from the centre of the screen even if alt-tabbed out.
		 *
		 * The delta time is necessary since you want to be able to move the
		 * same amount whether you have higher frames or lower frames.
		 * Multiplying the movement factor by the delta time means the
		 * amount moved within a specific amount of time is normalized and
		 * will not affect gameplay or create some form of confusion as some
		 * people go flying around.
		 */
		void tick(float dt);

		/**
		 * @brief Enables or disables the camera.
		 * @param enabled Whether to enable or disable the camera.
		 *
		 * You must use this function, using false, to gain access over the
		 * cursor, otherwise the tick function will always grab control of
		 * the mouse and centre it every frame, making it impossible to do
		 * anything.
		 */
		void enable(bool enabled);

		/**
		 * @brief Tells whether the camera is enabled or not.
		 * @return If the camera is enabled or not.
		 */
		bool isEnabled() const { return m_enabled; }

		/**
		 * @brief Recalculates the projection if the window is resized.
		 * @param e The event in which the window resize data is active.
		 *
		 * When an event is passed to this function, it should check that
		 * the event is actually a window resize event. If not checked, this
		 * function will segfault due to improper memory access, and might
		 * be difficult to trace.
		 */
		void onWindowResize(events::Event e);

		/**
		 * @brief Sets the actor for the camera to "follow".
		 * @param actor The actor to set the camera to use.
		 */
		void setActor(entt::entity actor);

		/**
		 * @brief Sets the world for the camera.
		 * @param world 
		 */
		void setWorld(voxels::ChunkView* world);

		/**
		 * @brief Set the Flying mode
		 * 
		 * @param value default to true
		 */
		void setFly(bool value);

		/**
		 * @brief Return true if the camera is in flying mode
		 * 
		 * @return true 
		 * @return false 
		 */
		bool fly() const;

	private:
		Window* m_window;

		math::mat4 m_projection;
		math::vec3 m_up;

		math::vec2 m_windowCentre;

		// Also used by InputQueue to determine if camera is focused or not
		std::atomic<bool> m_enabled;

		Setting* m_settingSensitivity;

		entt::registry* m_registry;
		entt::entity    m_actor;

<<<<<<< HEAD
		voxels::ChunkView* m_world;

		bool m_fly = true;
=======
		/// @todo Relocate player movement to player instead of camera
		client::Input* m_forward;
		client::Input* m_backward;
		client::Input* m_left;
		client::Input* m_right;
		client::Input* m_fly;
		client::Input* m_down;
		voxels::ChunkView* m_world;
>>>>>>> 7c7b5f2d7bedaffa4bd06c29fc255c47ec5b36a7
	};
} // namespace phx::gfx

