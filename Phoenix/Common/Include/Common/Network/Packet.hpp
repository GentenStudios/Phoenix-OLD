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

#include <Common/EnumTools.hpp>
#include <Common/Network/Types.hpp>

#include <enet/enet.h>

#include <cstddef>
#include <functional>
#include <vector>

namespace phx::net
{
	/**
	 * @brief Flags for deciding Packet flow.
	 */
	enum class PacketFlags
	{
		/**
		 * @brief The packet must use reliable delivery.
		 *
		 * A reliable packet is guaranteed to be delivered, and retries will be
		 * attempted until an acknowledgement (ack) is received. If a certain
		 * number of retries fail, a disconnection will be assumed and the
		 * connection will be forcefully reset.
		 *
		 * Reliable delivery is slow, if a majority of reliable is required, use
		 * a TCP library rather than ENet, which is designed with unreliable
		 * delivery of UDP packets in mind.
		 */
		RELIABLE = ENET_PACKET_FLAG_RELIABLE,

		/**
		 * @brief The packet will be sent unreliably.
		 *
		 * No retry attempts will be made, nor will and acks be received.
		 */
		UNRELIABLE = ENET_PACKET_FLAG_SENT,
	};

	/**
	 * @brief Represents a packet that will be sent to a host/peer.
	 */
	class Packet
	{
	public:
		using Data = std::vector<std::byte>;

	public:
		Packet() = default;

		/**
		 * @brief Constructs a packet with some default data.
		 * @param data The data to send within the packet.
		 * @param flags The method with which the packet should be sent.
		 */
		Packet(const Data& data, PacketFlags flags);

		/**
		 * @brief Constructs a packet with a predetermined size.
		 * @param size The size of the data which will be set later.
		 * @param flags The method with which the packet should be sent.
		 */
		Packet(std::size_t size, PacketFlags flags);

		// internal use.
		Packet(ENetPacket& packet, bool sent);

		~Packet();

		/**
		 * @brief Sets the data the packet will have.
		 * @param data The data to set within the packet.
		 *
		 * This method will implicitly resize the packet if required.
		 */
		void setData(const Data& data);
		Packet& operator=(const Data& data);

		/// @todo Implement a stream (<<) operator for adding data.

		/*
		 * @brief Gets the data the packet is storing.
		 * @return The packet's data.
		 *
		 * This method is useful on the receiving end. It will return a copy of
		 * the data since the packet's lifetime cannot be determined.
		 */
		Data getData() const;

		/**
		 * @brief Resizes the packet.
		 * @param size The new size for the packet.
		 */
		void        resize(std::size_t size);
		
		/**
		 * @brief Gets the size of the packet.
		 * @return The size of the packet.
		 */
		std::size_t getSize() const;

		operator ENetPacket*() const { return m_packet; }

		/**
		 * @brief Prepares a packet for sending.
		 *
		 * Note: this currently doesn't do much but set a variable, but this
		 * variable is important to guarantee that a packet is only destroyed
		 * once ready. Without this, the destructor may prematurely delete the
		 * packet's data.
		 */
		void prepareForSend();
		
		/**
		 * @brief Checks whether the packet has been sent.
		 * @return Whether the packet has been sent or not.
		 */
		bool isSent() const { return m_sent; }

	private:
		void create(const Data& data, PacketFlags flags);

	private:
		ENetPacket* m_packet;
		bool        m_sent = false;
	};
} // namespace phx::net

ENABLE_BITWISE_OPERATORS(phx::net::PacketFlags);
