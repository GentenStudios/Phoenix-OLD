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
	struct Duration
	{
		unsigned int minutes;
		unsigned int seconds;
	};

	struct AudioData;
	class Source
	{
	public:
		Source();
		explicit Source(AudioData data);
		~Source();

		void enableSpatial(bool enabled);
		void enableLoop(bool enabled);

		void setPos(math::vec3 pos);
		void setDirection(math::vec3 direction);
		void setVelocity(math::vec3 velocity);
		void setGain(float gain);
		void setPitch(float pitch);

		Duration getDuration() const;

		void setAudioData(AudioData buffer);
		void play() const;

	private:
		unsigned int m_source = 0;

		math::vec3 m_position;
		math::vec3 m_direction;
		math::vec3 m_velocity;
		float      m_gain  = 1.f;
		float      m_pitch = 1.f;
		Duration   m_duration;
	};
} // namespace phx::audio
