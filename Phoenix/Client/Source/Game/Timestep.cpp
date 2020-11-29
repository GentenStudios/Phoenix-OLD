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

#include <Client/Game/Timestep.hpp>

#include <SDL.h>

using namespace phx::client;

Timestep::Timestep()
{
	m_last = static_cast<float>(SDL_GetPerformanceCounter());
}

Timestep& Timestep::step()
{
	const auto now = static_cast<float>(SDL_GetPerformanceCounter());

	// m_time is now delta time in SECONDS.
	m_time = static_cast<float>(now - m_last) /
	         static_cast<float>(SDL_GetPerformanceFrequency());

	m_last = now;
	
	return *this;
}

void Timestep::clear()
{
	// this will just mean that if you call clear, you can essentially eliminate
	// any lag.
	m_last = static_cast<float>(SDL_GetPerformanceCounter());
	m_time = 0.f;
}

FixedTimestep::FixedTimestep(float timestep) : m_timestep(timestep)
{
	m_last = static_cast<float>(SDL_GetPerformanceCounter());
}

FixedTimestep& FixedTimestep::step()
{
	const float now = static_cast<float>(SDL_GetPerformanceCounter());

	// m_time is delta time in seconds.
	m_time = static_cast<float>(now - m_last) /
	         static_cast<float>(SDL_GetPerformanceFrequency());

	m_accumulator += m_time;

	m_last = now;

	return *this;
}

void FixedTimestep::clear()
{
	m_accumulator = 0.f;
	m_time        = 0.f;
	m_last        = SDL_GetPerformanceCounter();
}

bool FixedTimestep::shouldStep()
{
	if (m_accumulator >= m_timestep)
	{
		m_accumulator -= m_timestep;

		return true;
	}

	return false;
}
