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

#include <Client/Audio/Audio.hpp>

#include <Common/Logger.hpp>

#include <AL/al.h>
#include <AL/alc.h>

#pragma warning(disable : 4267 4244)
#define MINIMP3_ONLY_MP3
#define MINIMP3_IMPLEMENTATION
#include <minimp3/minimp3.h>
#include <minimp3/minimp3_ex.h>

using namespace phx::audio;

ALCcontext* Audio::m_context = nullptr;
ALCdevice*  Audio::m_device  = nullptr;
mp3dec_t    Audio::m_mp3;

Audio::~Audio()
{
	// deletes all buffers of registered sounds.
	for (auto& it : m_buffers)
	{
		alDeleteBuffers(1, &it.second.buffer);
	}
}

bool Audio::initialize()
{
	// opens an audio device.
	m_device = alcOpenDevice(nullptr);
	if (!m_device)
	{
		LOG_FATAL("AUDIO")
		    << "Could not open an audio device! No audio will be played.";
		return false;
	}

	// creates an OpenAL context.
	m_context = alcCreateContext(m_device, nullptr);
	if (!m_context)
	{
		LOG_FATAL("AUDIO")
		    << "Could not create an audio context! No audio will be played.";

		alcCloseDevice(m_device);
		return false;
	}

	// makes the context current.
	if (alcMakeContextCurrent(m_context) == ALC_FALSE)
	{
		LOG_FATAL("AUDIO")
		    << "Could not create an audio context! No audio will be played.";

		alcDestroyContext(m_context);
		alcCloseDevice(m_device);
		return false;
	}

	// initializes the MP3 loaded.
	mp3dec_init(&m_mp3);

	// place some initial values for the listener, this is all changeable and
	// should be immediately changed to the actual position of the player once
	// instantiated.
	ALfloat listenerPos[] = {0.0, 0.0, 0.0};
	ALfloat listenerVel[] = {0.0, 0.0, 0.0};
	ALfloat listenerOri[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	return true;
}

void Audio::teardown()
{
	// destroys the context and closes the audio device.
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_context);
	alcCloseDevice(m_device);
}

void Audio::loadMP3(const std::string& uniqueName, const std::string& filePath)
{
	// checks if uniqueName already exists, if so, then overwrite what's there.
	const auto it = m_buffers.find(uniqueName);
	if (it != m_buffers.end())
	{
		LOG_WARNING("AUDIO") << "The sound with the unique name: " << uniqueName
		                     << " already exists, but will be overwritten.";
	}

	// load the mp3 file. will just return empty if the file cannot be read.
	mp3dec_file_info_t info;
	if (mp3dec_load(&m_mp3, filePath.c_str(), &info, nullptr, nullptr))
	{
		LOG_FATAL("AUDIO")
		    << "An unexpected (but recoverable) error occurred while loading: "
		    << filePath;

		return;
	}

	// calculates the size of the buffer by the amount of samples and the size of each sample.
	const unsigned int bufferSize = info.samples * sizeof(mp3d_sample_t);

	// gets the duration of the audio, initially as a simple float, but then converted to minutes and seconds.
	const float durationSeconds = static_cast<float>(bufferSize) / (static_cast<float>(info.avg_bitrate_kbps) * 1024.f);
	const Duration duration     = {
        static_cast<unsigned int>(durationSeconds / 60.f),
        static_cast<unsigned int>(durationSeconds) % 60};

	unsigned int buffer;

	// if uniqueName wasn't found, the buffer doesn't exist, otherwise overwrite
	// the existing buffer to prevent undefined behaviour and wasted memory.
	if (it == m_buffers.end())
	{
		alGenBuffers(1, &buffer);
	}
	else
	{
		buffer = it->second.buffer;
	}

	// fill the buffer with the data.
	alBufferData(buffer,
	             info.channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
	             info.buffer, bufferSize, info.hz);

	// checks for an error.
	if (alGetError() != AL_NO_ERROR)
	{
		LOG_FATAL("AUDIO")
		    << "An unexpected (but recoverable) error occurred while loading: "
		    << filePath;

	}

	// adds the buffer ID and duration to the unordered_map, where it is
	// associated with the uniqueName. this can then be retrieved in
	// getAudioData or [].
	m_buffers.insert_or_assign(uniqueName, AudioData {buffer, duration});
}

AudioData Audio::getAudioData(const std::string& uniqueName) const
{
	auto it = m_buffers.find(uniqueName);
	if (it == m_buffers.end())
	{
		// if not found, return some dummy data.
		return {static_cast<unsigned int>(-1), {0, 0}};
	}

	return it->second;
}
