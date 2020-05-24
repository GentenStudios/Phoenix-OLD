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

#include <Client/Audio/Source.hpp>
#include <Client/Audio/Audio.hpp>

#include <AL/al.h>
#include <AL/alext.h>

using namespace phx::audio;

Source::Source()
{
	// creates an OpenAL source, ready for setting a buffer and playing back.
	alGenSources(1, &m_source);
}

Source::Source(AudioData data)
{
	// generates an OpenAL source and sets the buffer to be used during
	// playback.
	alGenSources(1, &m_source);
	alSourcei(m_source, AL_BUFFER, data.buffer);
	m_duration = data.duration;
}

Source::~Source()
{
	// deletes the OpenAL source that was created.
	// this will NOT delete the buffer (the storage for the actual audio).
	alDeleteSources(1, &m_source);
}

void Source::enableSpatial(bool enable)
{
	alSourcei(m_source, AL_SOURCE_SPATIALIZE_SOFT,
	          enable ? AL_TRUE : AL_FALSE);
}

void Source::enableLoop(bool enable)
{
	alSourcei(m_source, AL_LOOPING, enable ? AL_TRUE : AL_FALSE);
}

void Source::setPos(phx::math::vec3 pos)
{
	m_position = pos;
	alSourcefv(m_source, AL_POSITION, &m_position.x);
}

void Source::setDirection(phx::math::vec3 direction)
{
	m_direction = direction;
	alSourcefv(m_source, AL_DIRECTION, &m_direction.x);
}

void Source::setVelocity(phx::math::vec3 velocity)
{
	m_velocity = velocity;
	alSourcefv(m_source, AL_VELOCITY, &velocity.x);
}

void Source::setGain(float gain)
{
	m_gain = gain;
	alSourcef(m_source, AL_GAIN, gain);
}

void Source::setPitch(float pitch)
{
	m_pitch = pitch;
	alSourcef(m_source, AL_PITCH, pitch);
}

Duration Source::getDuration() const { return m_duration; }

Source::State Source::status() const
{
	int state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);

	return static_cast<State>(state);
}

void Source::setAudioData(AudioData data)
{
	m_duration = data.duration;
	alSourcei(m_source, AL_BUFFER, data.buffer);
}

void Source::play() const { alSourcePlay(m_source); }
