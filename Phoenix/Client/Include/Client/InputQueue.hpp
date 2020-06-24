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

#include <Client/InputMap.hpp>
#include <Client/Network.hpp>
#include <Client/Player.hpp>

#include <Common/Util/BlockingQueue.hpp>

namespace phx::client
{
	class InputQueue
	{
	public:
		InputQueue(entt::registry* registry, Player* player);
		~InputQueue();

		/**
		 * @brief Thread to capture and queue input states
		 * @param dt How frequently in seconds a state should be captured in
		 * milliseconds
		 * @param network Pointer to network to send states to
		 */
	private:
		void run(std::chrono::milliseconds dt, client::Network* network);

	public:
		/**
		 * @brief Starts a new thread to capture and queue input states
		 * @param dt How frequently in seconds a state should be captured in
		 * milliseconds
		 * @param network Pointer to network to send states to
		 */
		void start(std::chrono::milliseconds dt, client::Network* network);

		/**
		 * @brief Stops the current thread
		 */
		void stop();

		/**
		 * @brief Gets the current state of the input
		 * @return The input state
		 */
		InputState getCurrentState();

		BlockingQueue<InputState> m_queue;

	private:
		bool        m_running;
		std::thread m_thread;

		Player*         m_player;
		entt::registry* m_registry;

		std::size_t m_sequence;

		client::Input* m_forward;
		client::Input* m_backward;
		client::Input* m_left;
		client::Input* m_right;
		client::Input* m_up;
		client::Input* m_down;
	};
} // namespace phx::client