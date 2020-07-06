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

#include <Common/Logger.hpp>
#include <Common/Network/Host.hpp>

#include <utility>

using namespace phx::net;

std::atomic<std::size_t> Host::m_activeInstances = 0;

Host::Host(std::size_t peers, const ENetAddress* address)
    : Host(address ? Address {*address} : Address {}, peers)
{
}

Host::Host(const Address& address, std::size_t peers, std::size_t channels)
{
	if (m_activeInstances == 0)
	{
		if (enet_initialize())
		{
			LOG_FATAL("NETCODE") << "Failed to initialize ENet networking.";
			exit(EXIT_FAILURE);
		}
	}

	++m_activeInstances;

	m_host = enet_host_create(address, peers, channels, 0, 0);
}

Host::~Host()
{
	--m_activeInstances;

	enet_host_destroy(m_host);

	if (m_activeInstances == 0)
	{
		enet_deinitialize();
	}
}

Host::OptionalPeer Host::connect(const Address& address)
{
	return connect(address, getChannelLimit());
}

Host::OptionalPeer Host::connect(const Address& address, enet_uint8 channels,
                                 enet_uint32 data)
{
	ENetPeer* peer = enet_host_connect(m_host, address, channels, data);

	if (!peer)
	{
		LOG_FATAL("Failed to connect to peer.");
		return {};
	}

	return createPeer(*peer);
}

Bandwidth Host::getBandwidthLimit() const
{
	return {m_host->incomingBandwidth, m_host->outgoingBandwidth};
}

void Host::setBandwidthLimit(const Bandwidth& bandwidth)
{
	enet_host_bandwidth_limit(m_host, bandwidth.incoming, bandwidth.outgoing);
}

std::size_t Host::getChannelLimit() const { return m_host->channelLimit; }

void Host::setChannelLimit(std::size_t limit)
{
	enet_host_channel_limit(m_host, limit);
}

void Host::broadcast(Packet& packet, enet_uint8 channel)
{
	packet.prepareForSend();
	enet_host_broadcast(m_host, channel, packet);
}

void Host::broadcast(Packet&& packet, enet_uint8 channel)
{
	packet.prepareForSend();
	enet_host_broadcast(m_host, channel, packet);
}

void Host::onReceive(ReceiveCallback callback)
{
	m_receiveCallback = std::move(callback);
}

void Host::onConnect(ConnectCallback callback)
{
	m_connectCallback = std::move(callback);
}

void Host::onDisconnect(DisconnectCallback callback)
{
	m_disconnectCallback = std::move(callback);
}

void Host::poll(int limit) { poll(0_ms, limit); }

void Host::poll(phx::time::ms timeout, int limit)
{
	ENetEvent event;

	do
	{
		if (enet_host_service(m_host, &event, timeout.count()))
		{
			handleEvent(event);
		}
	} while (--limit);
}

void Host::flush() { enet_host_flush(m_host); }

std::size_t Host::getPeerCount() const { return m_host->connectedPeers; }

std::size_t Host::getPeerLimit() const { return m_host->peerCount; }

const Address& Host::getAddress() const { return m_address; }

Peer* Host::getPeer(std::size_t id)
{
	if (m_peers.find(id) != m_peers.end())
	{
		return &m_peers[id];
	}
	return nullptr;
}

enet_uint32 Host::getTotalReceievedData() const
{
	return m_host->totalReceivedData;
}

enet_uint32 Host::getTotalSentData() const { return m_host->totalSentData; }

void Host::removePeer(const Peer& peer)
{
	removePeer(*static_cast<ENetPeer*>(peer));
}

void Host::handleEvent(ENetEvent& event)
{
	ENetPeer* peer = event.peer;

	switch (event.type)
	{
	case ENET_EVENT_TYPE_CONNECT:
		if (m_connectCallback)
		{
			m_connectCallback(createPeer(*peer), event.data);
		}
		break;

	case ENET_EVENT_TYPE_RECEIVE:
		if (m_receiveCallback)
		{
			m_receiveCallback(getPeer(*peer), Packet(*event.packet, true),
			                  event.channelID);
		}

		enet_packet_destroy(event.packet);
		break;

	case ENET_EVENT_TYPE_DISCONNECT:
		if (m_disconnectCallback)
		{
			m_disconnectCallback(std::size_t(peer->data), event.data);
		}
		removePeer(*peer);
		break;

	default:
		break;
	}
}

Peer& Host::getPeer(ENetPeer& peer)
{
	if (m_peers.find(std::size_t(peer.data)) != m_peers.end())
	{
		return m_peers.at(std::size_t(peer.data));
	}
	LOG_WARNING("NETCODE") << "peer not found";
}

Peer& Host::createPeer(ENetPeer& peer)
{
	++m_peerID;
	peer.data = reinterpret_cast<void*>(m_peerID);
	m_peers.insert({m_peerID, {*this, peer}});
	return m_peers.at(m_peerID);
}

void Host::removePeer(ENetPeer& peer)
{
	auto id = std::size_t(peer.data);
	m_peers.erase(id);

	peer.data = nullptr;
}

void Host::disconnectPeer(std::size_t id)
{
	if (m_disconnectCallback)
	{
		m_disconnectCallback(id, 0);
	}

	removePeer(m_peers.at(id));
}
