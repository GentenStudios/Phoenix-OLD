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

#include <AL/al.h>

#include <atomic>

namespace phx::audio
{
	/**
	 * @brief A struct holding the duration of an audio clip, in minutes and
	 * seconds.
	 *
	 * @ref AudioData
	 */
	struct Duration
	{
		unsigned int minutes = 0;
		unsigned int seconds = 0;
	};

	struct AudioData;

	/**
	 * @brief The source of audio.
	 *
	 * This class represents an audio source, like a speaker or a sheep...
	 *
	 * The source initially has spatial support disabled, as well as looping.
	 * This is idea mainly for things like background music, since they can be
	 * looped and shouldn't be affected by the player's position.
	 *
	 * @paragraph Usage
	 * @code
	 * audio::Audio manager;
	 * manager.loadMP3("core:song", "Assets/song.mp3");
	 * manager.loadMP3("core:song2", "Assets/song.mp3");
	 *
	 * audio::Source source;
	 * source.setAudioData(manager["core:song"]);
	 * source.enableSpatial(true);
	 * source.enableLoop(true);
	 *
	 * // one is same as input.
	 * source.setGain(1.f);
	 * source.setPitch(1.f);
	 *
	 * source.play();
	 *
	 * // wait for source to finish playing whatever it's playing.
	 * while (source.status() != Source::State::Stopped)
	 * {
	 *     ;
	 * }
	 *
	 * source.setAudioData(manager["core:song2"]);
	 * ...
	 * @endcode
	 */
	class Source
	{
	private:
		struct Deleter
		{
			void operator()(unsigned int* source) const
			{
				alDeleteSources(1, source);
				delete source;
			}
		};
		
	public:
		/**
		 * @brief The current playing state of the Source.
		 */
		enum class State
		{
			/**
			 * @brief The Source is playing.
			 */
			PLAYING = AL_PLAYING,

			/**
			 * @brief The source is paused.
			 */
			PAUSED = AL_PAUSED,

			/**
			 * @brief The source is stopped, so either it hasn't been started,
			 * or it has finished.
			 */
			STOPPED = AL_STOPPED
		};

	public:
		/**
		 * @brief Constructs a Source object.
		 */
		Source();

		/**
		 * @brief Constructs a Source object but with AudioData from the start.
		 * @param data The audio data to use.
		 *
		 * This one is useful if you know exactly what audio needs to be played.
		 * The AudioData can be set later on so using this constructor is not
		 * particularly necessary.
		 */
		explicit Source(AudioData data);

		~Source() = default;

		/**
		 * @brief Allows for enabling/disabling spatial audio as required.
		 * @param enable Whether to enable spatial audio or not.
		 *
		 * Spatial audio is disabled by default. This is useful for things like
		 * background music, etc... Spatial audio is required for position-based
		 * audio, such as surrounding entities, world audio, etc...
		 */
		void enableSpatial(bool enable);

		/**
		 * @brief Allows for enabling/disabling the audio from looping.
		 * @param enable Whether to enable looping or not.
		 *
		 * Looping is disabled by default, since most sounds are a
		 * play-on-demand rather than all the time. Things like ambient audio or
		 * background music are more likely to be looped, so enabling this is
		 * necessary.
		 */
		void enableLoop(bool enable);

		/**
		 * @brief Sets the position of the source.
		 * @param position The position of the source.
		 */
		void setPos(math::vec3 position);

		/**
		 * @brief Sets the direction of the source.
		 * @param direction The direction the source is facing.
		 */
		void setDirection(math::vec3 direction);

		/**
		 * @brief Sets the velocity of the source.
		 * @param velocity The velocity of the source.
		 *
		 * Velocity is a vector quantity, this means it contains direction and
		 * magnitude. The velocity is a combination of speed and the direction
		 * in which it is moving.
		 *
		 * Velocity is also important in the applying the "Doppler Effect".
		 */
		void setVelocity(math::vec3 velocity);

		/**
		 * @brief Sets the gain of the source.
		 * @param gain The gain the audio will be played back at.
		 */
		void setGain(float gain);

		/**
		 * @brief Sets the pitch of the source.
		 * @param pitch The pitch the audio will be played back at.
		 */
		void setPitch(float pitch);

		/**
		 * @brief Returns the duration of the audio clip.
		 * @return The duration of the audio clip.
		 *
		 * This can return 0, 0 due to the seconds being an unsigned int,
		 * lacking the precision of a float.
		 *
		 * @todo Increase the resolution of Duration, to allow for telling the
		 * duration of clips shorter than a second.
		 */
		Duration getDuration() const;

		/**
		 * @brief Returns the playing status of the source.
		 * @return The status of the source: playing, paused or finished.
		 */
		State status() const;

		/**
		 * @brief Sets the audio for the source to use, comes from audio::Audio.
		 * @param buffer The data associated to the audio requested.
		 *
		 * This method should be used with the audio::Audio class.
		 * audio::Audio::getAudioData("core:song") will return an AudioData
		 * object for the source to use and play.
		 */
		void setAudioData(AudioData buffer);

		/**
		 * @brief Plays the audio. Calling again will pause.
		 */
		void play() const;

		/**
		 * @brief Pauses the audio.
		 */
		void pause() const;

		/**
		 * @brief Stops the audio.
		 */
		void stop() const;

	private:
		// internal variable for OpenAL.
		std::shared_ptr<unsigned int> m_source;
		Duration     m_duration;
	};
} // namespace phx::audio
