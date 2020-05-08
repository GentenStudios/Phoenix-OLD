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

#pragma once

#include <Common/Math/Math.hpp>

namespace phx::audio
{
	/**
	 * @brief The listener, the player side of positional audio.
	 *
	 * This is basically a dummy class. You can only have one listener per
	 * OpenAL context, and it's global. There are no variables, just OpenAL
	 * calls within these functions. The reason this exists is to provide an
	 * easier, more intuitive way to control the player's positional data.
	 *
	 * @paragraph Usage
	 * @code
	 * audio::Audio manager;
	 * manager.loadMP3("core:cow_going_moo_for_1_minute", "Assets/moo.mp3");
	 *
	 * audio::Source source;
	 * source.setAudioData(manager["core:cow_going_moo_for_1_minute"]);
	 *
	 * source.enableSpatial(true);
	 * source.setPosition({ 0.f, 10.f, 0.f });
	 *
	 * source.play();
	 *
	 * audio::Listener* listener = manager.getListener();
	 * listener.setPosition({ 0.f, 0.f, 0.f });
	 *
	 * // pseudo code
	 * thread::sleep(10 seconds);
	 *
	 * listener.setPosition({ 10.f, 0.f, 0.f });
	 *
	 * // pseudo code
	 * thread::sleep(10 seconds);
	 *
	 * same thing continued...
	 */
	class Listener
	{
	public:
		/**
		 * @brief Sets the gain the listener will use.
		 * @param gain The gain everything should be finally adjusted to.
		 *
		 * Using a scale of 0 to 1.0, this can be used as a way to control
		 * volume.
		 */
		void setGain(float gain);
		
		/**
		 * @brief Sets the position of the listener.
		 * @param position The position of the listener.
		 */
		void setPosition(math::vec3 position);
		
		/**
		 * @brief Sets the velocity of the listener.
		 * @param velocity The velocity of the listener.
		 *
		 * Velocity is a vector quantity, this means it contains direction and
		 * magnitude. The velocity is a combination of speed and the direction
		 * in which it is moving.
		 *
		 * Velocity is also important in the applying the "Doppler Effect".
		 */
		void setVelocity(math::vec3 velocity);

		/**
		 * @brief Sets the orientation of the player.
		 * @param direction The direction of the player.
		 * @param up The up vector of the player (what way up they are.)
		 */
		void setOrientation(math::vec3 direction, math::vec3 up);

	private:
		// private constructor because you shouldn't be able to make a Listener.
		Listener()  = default;
		~Listener() = default;

		friend class Audio;
	};
} // namespace phx::audio
