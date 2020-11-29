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

namespace phx::client
{
	class Timestep
	{
	public:
		Timestep();

		Timestep& step();
		void      clear();

		// will return seconds.
		operator float() const { return m_time; }

		float getSeconds() const { return m_time; }
		float getMilliseconds() const { return m_time * 1000; }
		float getMicroseconds() const { return m_time * 1000000; }

	private:
		float m_time = 0.f;
		float m_last = 0.f;
	};

	class FixedTimestep
	{
	public:
		static constexpr float FPS_60 = 0.016666667f;
		
	public:
		FixedTimestep(float timestep);

		FixedTimestep& step();
		void           clear();

		bool shouldStep();

		operator float() const { return m_time; }

		float getSeconds() const { return m_time; }
		float getMilliseconds() const { return m_time * 1000; }
		float getMicroseconds() const { return m_time * 1000000; }

	private:
		float m_timestep;
		float m_last;
		
		float m_accumulator = 0.f;
		float m_time = 0.f;
	};
} // namespace phx::client
