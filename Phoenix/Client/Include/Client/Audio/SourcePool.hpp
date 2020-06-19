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

#include <Client/Audio/Audio.hpp>
#include <Client/Audio/Source.hpp>

#include <deque>
#include <vector>
#include <mutex>

namespace phx::audio
{
	class SourcePool
	{
	public:
		/**
		 * @brief The maximum amount of sources that can be played at once.
		 *
		 * This is set to 32 since that's the maximum concurrently playing
		 * sources on iOS devices. 32 should be fairly generous for a voxel game
		 * anyways.
		 */
		constexpr static std::size_t MAX_SIMULTANEOUS_SOURCES = 32;

	public:
		SourcePool();
		~SourcePool() = default;

		void queue(const Source& source);
		void queue(Source&& source);

		void forceNext(const Source& source);
		void forceNext(Source&& source);

		void clear();
		std::size_t playingCount() const;
		
		void pause();
		void play();
		void stop();

		void tick();

	private:
		bool m_paused = false;
		
		std::mutex m_mutex;
		
		std::deque<Source> m_sourcesToPlay;
		std::vector<Source> m_playingSources;
	};
} // namespace phx::audio
