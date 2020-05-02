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

#include <Common/Network/Address.hpp>
#include <Common/Network/Packet.hpp>
#include <Common/Network/Types.hpp>

#include <enet/enet.h>

#include <chrono>

namespace phx::net
{
	enum class PeerStatus
	{
		CONNECTING    = ENET_PEER_STATE_CONNECTING,
		CONNECTED     = ENET_PEER_STATE_CONNECTED,
		DISCONNECTING = ENET_PEER_STATE_DISCONNECTING,
		DISCONNECTED  = ENET_PEER_STATE_DISCONNECTED,
	};

	class Host;

	namespace detail
	{
		// not for user use
		struct PeerData
		{
			void*       data;
			std::size_t id;
		};
	} // namespace detail

	class Peer
	{
	public:
		using Callback = std::function<void(const Packet&, enet_uint8)>;

	public:
		Peer() = default;
		Peer(Host& host, ENetPeer& peer);
		Peer(const Peer& other) = default;

		Peer& operator=(ENetPeer& peer);
		Peer& operator=(const Peer& other) = default;

		void disconnect(enet_uint32 data = 0);
		void disconnectImmediately(enet_uint32 data = 0);
		void disconnectOncePacketsAreSent(enet_uint32 data = 0);
		void drop();

		void        ping() const;
		time::ms    getPingInterval() const;
		void        setPingInterval(time::ms interval);
		time::ms    getRoundTripTime() const;
		enet_uint32 getPacketLoss() const;

		void receive(Callback callback) const;

		void send(Packet& packet, enet_uint8 channel = 0);
		void send(Packet&& packet, enet_uint8 channel = 0);

		Throttle getThrottle() const;
		void     setThrottle(const Throttle& throttle);

		Timeout getTimeout() const;
		void    setTimeout(const Timeout& timeout);

		const Address& getAddress() const;
		PeerStatus     getState() const;
		std::size_t    getID() const { return std::size_t(m_peer->data); }

		operator ENetPeer*() const { return m_peer; }

	private:
		ENetPeer* m_peer;

		Host*   m_host;
		Address m_address;
	};
} // namespace phx::net
