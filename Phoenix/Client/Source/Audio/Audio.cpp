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
#define MINIMP3_IMPLEMENTATION
#include <minimp3/minimp3.h>
#include <minimp3/minimp3_ex.h>

using namespace phx::audio;

ALCcontext* Audio::m_context = nullptr;
ALCdevice*  Audio::m_device  = nullptr;
mp3dec_t    Audio::m_mp3;

Audio::~Audio() { teardown(); }

bool Audio::initialize()
{
	m_device = alcOpenDevice(nullptr);
	if (!m_device)
	{
		LOG_FATAL("AUDIO")
		    << "Could not open an audio device! No audio will be played.";
		return false;
	}

	m_context = alcCreateContext(m_device, nullptr);
	if (!m_context)
	{
		LOG_FATAL("AUDIO")
		    << "Could not create an audio context! No audio will be played.";

		alcCloseDevice(m_device);
		return false;
	}

	if (alcMakeContextCurrent(m_context) == ALC_FALSE)
	{
		LOG_FATAL("AUDIO")
		    << "Could not create an audio context! No audio will be played.";

		alcDestroyContext(m_context);
		alcCloseDevice(m_device);
		return false;
	}

	mp3dec_init(&m_mp3);

	// Init listener
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
	if (m_context == nullptr)
	{
		return;
	}

	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_context);
	alcCloseDevice(m_device);
}

void Audio::loadMP3(const std::string& uniqueName, const std::string& filePath)
{
	auto it = m_buffers.find(uniqueName);
	if (it != m_buffers.end())
	{
		LOG_WARNING("AUDIO") << "The sound with the unique name: " << uniqueName
		                     << " already exists, but will be overwritten.";
	}

	mp3dec_file_info_t info;
	int result = mp3dec_load(&m_mp3, filePath.c_str(), &info, nullptr, nullptr);

	const unsigned int bufferSize = info.samples * sizeof(mp3d_sample_t);

	const float durationSeconds = bufferSize / (info.avg_bitrate_kbps * 1024.f);
	const Duration duration     = {
        static_cast<unsigned int>(durationSeconds / 60.f),
        static_cast<unsigned int>(durationSeconds) % 60};

	unsigned int buffer;
	alGenBuffers(1, &buffer);
	alBufferData(buffer,
	             info.channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
	             info.buffer, bufferSize, info.hz);

	if (alGetError() != AL_NO_ERROR)
	{
		LOG_FATAL("AUDIO")
		    << "An unexpected (but recoverable) error occurred while loading: "
		    << filePath;
	}

	m_buffers.insert_or_assign(uniqueName, AudioData {buffer, duration});
}

AudioData Audio::getAudioData(const std::string& uniqueName) const
{
	auto it = m_buffers.find(uniqueName);
	if (it == m_buffers.end())
	{
		return {static_cast<unsigned int>(-1), 0u, 0u};
	}

	return it->second;
}
