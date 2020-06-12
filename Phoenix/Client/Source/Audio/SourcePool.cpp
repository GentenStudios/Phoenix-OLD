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

#include <Client/Audio/SourcePool.hpp>

#include <Common/Logger.hpp>

using namespace phx::audio;

SourcePool::SourcePool() { m_playingSources.reserve(MAX_SIMULTANEOUS_SOURCES); }

void SourcePool::queue(const Source& source)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_sourcesToPlay.push_back(source);
}

void SourcePool::queue(Source&& source)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_sourcesToPlay.emplace_back(std::move(source));
}

void SourcePool::forceNext(const Source& source)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_sourcesToPlay.push_front(source);
}

void SourcePool::forceNext(Source&& source)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_sourcesToPlay.emplace_front(source);
}

void SourcePool::clear()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_sourcesToPlay.clear();
}

std::size_t SourcePool::playingCount() const { return m_playingSources.size(); }

void SourcePool::pause()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_paused)
	{
		return;
	}

	m_paused = true;

	for (auto& source : m_playingSources)
	{
		source.pause();
	}
}

void SourcePool::play()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_paused)
	{
		return;
	}

	m_paused = false;

	for (auto& source : m_playingSources)
	{
		source.play();
	}
}

void SourcePool::stop()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (auto& source : m_playingSources)
	{
		source.stop();
	}

	m_playingSources.clear();
}

void SourcePool::tick()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (std::size_t i = 0; i < m_playingSources.size(); ++i)
	{
		// if stopped, this source has finished, lets replace it.
		if (m_playingSources[i].status() == Source::State::STOPPED)
		{
			// no more sources to add, lets just remove this element and carry
			// on with the rest of the work.
			if (m_sourcesToPlay.empty())
			{
				std::swap(m_playingSources[i], m_playingSources.back());
				m_playingSources.pop_back();

				// if there aren't any extra sources to add now, there won't be
				// for the rest of the loop so lets just leave this loop.
				break;
			}

			m_playingSources[i] = m_sourcesToPlay.front();
			m_sourcesToPlay.pop_front();

			m_playingSources[i].play();
		}
	}

	// if there are still sources to play and we still have space in the array
	// of playing sources, just keep 'em comin.
	while (m_playingSources.size() < MAX_SIMULTANEOUS_SOURCES &&
	       !m_sourcesToPlay.empty())
	{
		m_playingSources.push_back(m_sourcesToPlay.front());
		m_sourcesToPlay.pop_front();

		m_playingSources.back().play();
	}
}
