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
#include <Common/Network/Peer.hpp>

using namespace phx::net;

Peer::Peer(Host& host) : m_host(&host) {}

Peer::Peer(Host& host, ENetPeer& peer) : m_peer(&peer), m_host(&host) {}

Peer& Peer::operator=(ENetPeer& peer)
{
	m_peer    = &peer;
	m_address = peer.address;

	return *this;
}

void Peer::disconnect(enet_uint32 data) const
{
	enet_peer_disconnect(m_peer, data);
}

void Peer::disconnectImmediately(enet_uint32 data) const
{
	enet_peer_disconnect_now(m_peer, data);
	m_host->removePeer(*this);
}

void Peer::disconnectOncePacketsAreSent(enet_uint32 data) const
{
	enet_peer_disconnect_later(m_peer, data);
}

void Peer::drop() const
{
	enet_peer_reset(m_peer);
	m_host->removePeer(*this);
}

void Peer::ping() { enet_peer_ping(m_peer); }

phx::time::ms Peer::getPingInterval() const
{
	return phx::time::ms {m_peer->pingInterval};
}

void Peer::setPingInterval(phx::time::ms interval)
{
	enet_peer_ping_interval(m_peer, interval.count());
}

phx::time::ms Peer::getRoundTripTime() const
{
	return phx::time::ms {m_peer->roundTripTime};
}

enet_uint32 Peer::getPacketLoss() const { return m_peer->packetLoss; }

void Peer::receive(Callback callback) const
{
	enet_uint8 channel;
	auto       packet = enet_peer_receive(m_peer, &channel);
	callback(Packet {*packet, true}, channel);
}

void Peer::send(Packet& packet, enet_uint8 channel)
{
	packet.prepareForSend();
	enet_peer_send(m_peer, channel, packet);
}

void Peer::send(Packet&& packet, enet_uint8 channel)
{
	packet.prepareForSend();
	enet_peer_send(m_peer, channel, packet);
}

Throttle Peer::getThrottle() const
{
	return {phx::time::ms {m_peer->packetThrottleInterval},
	        m_peer->packetThrottleAcceleration,
	        m_peer->packetThrottleDeceleration};
}

void Peer::setThrottle(const Throttle& throttle)
{
	enet_peer_throttle_configure(m_peer, throttle.interval.count(),
	                             throttle.acceleration, throttle.deceleration);
}

Timeout Peer::getTimeout() const
{
	return {
	    phx::time::ms {m_peer->timeoutLimit},
	    phx::time::ms {m_peer->timeoutMinimum},
	    phx::time::ms {m_peer->timeoutMaximum},
	};
}

void Peer::setTimeout(const Timeout& timeout)
{
	enet_peer_timeout(m_peer, timeout.limit.count(), timeout.minimum.count(),
	                  timeout.maximum.count());
}

const Address& Peer::getAddress() const { return m_address; }

PeerStatus Peer::getState() const
{
	return static_cast<PeerStatus>(m_peer->state);
}
