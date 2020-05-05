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

#include <Common/Input.hpp>
#include <list>

namespace phx::client
{
	class InputQueue
	{
	public:
		InputQueue();

		/**
		 * @brief Thread to capture and queue input states
		 * @param dt How frequently in seconds a state should be captured
		 */
		void run(float dt);

		/**
		 * @brief Stops the current thread
		 */
		void kill();

		/**
		 * @brief Gets a state for processing
		 * @param sequence The sequence number for the desired state
		 * @return The input state
		 */
		InputState getState(std::size_t sequence);
		/**
		 * @brief Clears all states equal to or older than the supplied sequence
		 * @param sequence The sequence number to clear through
		 */
		void clearState(std::size_t sequence);
		/**
		 * @brief Gets the current sequence number, this is the most recent
		 * InputState that was queued
		 * @return A numerical identifier for the sequence
		 */
		std::size_t currentSequence();

	private:
		std::list<InputState> m_queue;
		bool                  m_running;

		client::Input* m_forward;
		client::Input* m_backward;
		client::Input* m_left;
		client::Input* m_right;
		client::Input* m_up;
		client::Input* m_down;
	};
} // namespace phx::client