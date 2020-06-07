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

#include <Client/Audio/Listener.hpp>
#include <Client/Audio/Source.hpp>

#include <AL/alc.h>
#include <minimp3/minimp3.h>

#include <unordered_map>

namespace phx::audio
{
	/**
	 * @brief Stores data to pass between the Audio engine and Sources.
	 */
	struct AudioData
	{
		/**
		 * @brief The buffer ID, more for internal use.
		 */
		unsigned int buffer;

		/**
		 * @brief The duration of the audio, minutes and seconds.
		 */
		Duration duration;
	};

	/**
	 * @brief Loads audio and provides an ability to play them.
	 *
	 * This class is to be used in conjunction with the Source class. Currently
	 * only MP3 is supported, future version may include OGG support.
	 *
	 * @paragraph Usage
	 * Before use, you must call ``initialize()`` to produce the OpenAL context,
	 * otherwise a barrage of errors will be displayed in the console. Before
	 * shutdown of the game, make sure all playing Sources are stopped and you
	 * have called the ``teardown()`` method to clear any existing buffers from
	 * memory.
	 *
	 * The destructor must be called before teardown, you can do this by
	 * initializing before a scope, and tearing down after the end of the scope,
	 * while instantiating the Audio class within the closed scope.
	 *
	 * @code
	 * using namespace phx;
	 *
	 * audio::Audio::initialize();
	 *
	 * {
	 *     audio::Audio manager;
	 *     manager.loadMP3("core:song1", "Assets/Song1.mp3");
	 *     manager.loadMP3("core:song2", "Assets/Song2.mp3");
	 *
	 *     audio::Source source;
	 *     source.setAudioData(manager.getAudioData("core:song1"));
	 *     // also possible to do: manager["core:song1"]
	 *
	 *     source.enableSpatial(true);
	 *     source.setPosition({ 10.f, 0.f, 0.f });
	 *
	 *     audio::Listener* listener = manager.getListener();
	 *     listener->setGain(...);
	 *     listener->setPosition(...);
	 *
	 *     source.play();
	 * }
	 * @endcode
	 */
	class Audio
	{
	public:
		using Handle = unsigned int;

	public:
		Audio() = default;

		/**
		 * @brief Deletes all existing buffers registered on this instance.
		 *
		 * Needs to be called before teardown, so consider creating the Audio
		 * object in a closed scope after initialization and teardown.
		 */
		~Audio();

		/**
		 * @brief Initializes the audio subsystem, creates an OpenAL context.
		 * @return true on success
		 * @return false on failure.
		 *
		 * This needs to be called before an Audio object is instantiated. This
		 * function creates an OpenAL context on a specific audio device. This
		 * cannot currently be chosen, it is automatic. Eventually, however,
		 * once a UI is added, extra methods will be implemented to allow the
		 * option to select an output device.
		 */
		static bool initialize();

		/**
		 * @brief Shuts down the audio subsystem.
		 *
		 * This needs to be called after all Audio objects have been destructed
		 * since the destructors delete all existing buffers. This function
		 * destroys the OpenAL context and closes the audio device - if all
		 * destructors are not called, there will be a list of errors outputted
		 * to console about existing buffers not being deleted.
		 */
		static void teardown();

		/**
		 * @brief Loads an MP3 into a buffer, ready for playing in a Source.
		 * @param uniqueName The unique name for the audio clip.
		 * @param filePath The path including the filename of the audio.
		 *
		 * This method does won't cause a segfault if an unexpected file is
		 * provided, however it will not load that file in, and an error will be
		 * logged. This function will load an MP3 file into an OpenAL buffer
		 * ready for use in Sources.
		 */
		Handle loadMP3(const std::string& uniqueName,
		               const std::string& filePath);

		const std::string& getUniqueName(Handle handle) const;
		
		/**
		 * @brief Gets the audio data for a Source to use before playing.
		 * @param uniqueName The unique name of the audio clip being retrieved.
		 * @return The buffer and duration for a valid unique name,
		 * alternatively, a dummy buffer and duration will be returned.
		 *
		 * This function will return a buffer of -1 if an invalid uniqueName is
		 * provided. Since it's an unsigned integer, -1 doesn't really exist,
		 * but checking for it will in an if statement, so check whether the
		 * buffer is -1 before using on an actual Source.
		 */
		AudioData getAudioData(const std::string& uniqueName) const;
		AudioData getAudioData(Handle handle) const;

		// [] operator, like an unordered_map, just redirected to getAudioData,
		// essentially just a QoL feature.
		AudioData operator[](const std::string& uniqueName) const
		{
			return getAudioData(uniqueName);
		}

		// [] operator, like an unordered_map, just redirected to getAudioData,
		// essentially just a QoL feature.
		AudioData operator[](Handle handle) const
		{
			return getAudioData(handle);
		}

		/**
		 * @brief The listener (you, what you hear in your headphones)
		 * @return The listener object.
		 */
		Listener* getListener() { return &m_listener; }

	private:
		static ALCcontext* m_context;
		static ALCdevice*  m_device;
		static mp3dec_t    m_mp3;

		Listener m_listener;

		// maps uniqueName from loadMP3 to a buffer id + duration.
		std::unordered_map<std::string, Handle> m_handles;
		std::unordered_map<Handle, Duration>    m_buffers;
	};
} // namespace phx::audio
