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
#include <Common/Network/Peer.hpp>
#include <Common/Network/Types.hpp>

#include <enet/enet.h>

#include <atomic>
#include <functional>
#include <optional>

namespace phx::net
{
	class Host
	{
	public:
		using ReceiveCallback =
		    std::function<void(Peer&, Packet&&, enet_uint8)>;
		using ConnectCallback = std::function<void(Peer&, enet_uint32)>;
		using DisconnectCallback =
		    std::function<void(void*, enet_uint32)>;

	public:
		Host(std::size_t peers = 1, const ENetAddress* address = nullptr);
		Host(const Address& address, std::size_t peers, std::size_t channels = 0);
		~Host();

		// check if value exists before using.
		using OptionalPeer = std::optional<std::reference_wrapper<Peer>>;
		OptionalPeer connect(const Address& address, enet_uint32 data = 0);

		Bandwidth getBandwidth() const;
		void      setBandwidth(const Bandwidth& bandwidth);

		std::size_t getChannelLimit() const;
		void        setChannelLimit(std::size_t limit);

		void broadcast(Packet& packet, enet_uint8 channel = 0);
		void broadcast(Packet&& packet, enet_uint8 channel = 0);

		void onReceive(ReceiveCallback callback);
		void onConnect(ConnectCallback callback);
		void onDisconnect(DisconnectCallback callback);

		void poll(int limit = 0);
		void poll(time::ms timeout, int limit = 0);

		void flush();

		std::size_t              getPeerCount() const;
		std::size_t              getPeerLimit() const;
		const std::vector<Peer>& getPeers() const;
		void                     removePeer(const Peer& peer);

		const Address& getAddress() const;

		enet_uint32 getTotalReceievedData() const;
		enet_uint32 getTotalSentData() const;

		operator ENetHost*() const { return m_host; }

	private:
		void handleEvent(ENetEvent& event);

		Peer& getPeer(ENetPeer& peer);
		Peer& createPeer(ENetPeer& peer);
		void removePeer(ENetPeer& peer);

	private:
		ENetHost* m_host;
		Address   m_address;

		ReceiveCallback    m_receiveCallback;
		ConnectCallback    m_connectCallback;
		DisconnectCallback m_disconnectCallback;

		std::vector<Peer> m_peers;

		static std::atomic<std::size_t> m_activeInstances;
	};
} // namespace phx::net
