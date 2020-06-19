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
#include <Common/Network/Host.hpp>
#include <Common/Util/BlockingQueue.hpp>

namespace phx::client
{
	class Network
	{
	public:
		Network(std::ostringstream& chat);
		~Network();

		void tick();

		void kill() { m_running = false; };

		/**
		 * @brief Actions taken when a state is received
		 *
		 * @param data The data in the state packet
		 * @param dataLength The length of the data in the state packet
		 */
		void parseEvent(net::Packet& packet);

		/**
		 * @brief Actions taken when a state is received
		 *
		 * @param data The data in the state packet
		 * @param dataLength The length of the data in the state packet
		 */
		void parseState(net::Packet& packet);

		/**
		 * @brief Actions taken when a message is received
		 *
		 * @param data The data in the message packet
		 * @param dataLength The length of the data in the message packet
		 */
		void parseMessage(phx::net::Packet& packet);

		/**
		 * @brief Sends a state packet to a client
		 *
		 * @param userRef The user to sent the state to
		 * @param data The state packet data
		 */
		void sendState(InputState inputState);

		/**
		 * @brief Sends a message packet to a client
		 *
		 * @param userRef The user to sent the message to
		 * @param data The message packet data
		 */
		void sendMessage(std::string message);

	private:
		bool                m_running;
		phx::net::Host*     m_client;
		std::ostringstream& m_chat;
	};
} // namespace phx::client::net