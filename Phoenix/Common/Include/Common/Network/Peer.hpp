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
#include <iostream>

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

	/**
	 * @brief Class to represent a Peer, a foreign host that has been connected
	 * to.
	 *
	 * This class is always instantiated by the Host class, a user must not
	 * construct a peer that is not default constructed. A default constructed
	 * peer must still NOT be used, all methods are unsafe until set to a valid
	 * Peer produced by a Host.
	 */
	class Peer
	{
	public:
		using Callback = std::function<void(const Packet&, enet_uint8)>;

	public:
		Peer() = default;
		Peer(Host& host, ENetPeer& peer);

		Peer& operator=(ENetPeer& peer);

		/**
		 * @brief Disconnects a peer from the host.
		 * @param data Data to send to the foreign host, use only if understood.
		 *
		 * Note: Host::poll must be called if you want a disconnect event to
		 * propagate. Use cases for this are the client disconnected it's peer
		 * (the server). The client must poll and wait for a disconnect event
		 * before quitting.
		 */
		void disconnect(enet_uint32 data = 0);

		/**
		 * @brief Disconnects a peer from the host IMMEDIATELY.
		 * @param data Data to send to the foreign host, use only if understood.
		 *
		 * Note: This will not produce an event on the host calling this
		 * function. For example, if the client disconnects it's peer (the
		 * server) using this method, it will manually have to tell itself that
		 * it has disconnected, it will not receive an event.
		 */
		void disconnectImmediately(enet_uint32 data = 0);

		/**
		 * @brief Disconnects a peer from the host once packets are sent.
		 * @param data Data to send to the foreign host, use only if understood.
		 *
		 * This function essentially doesn't do much different from the
		 * disconnect() method, however it will not read any events queued
		 * locally for processing, it will send any packets left to send, and
		 * then ditch any packets that may have been received.
		 */
		void disconnectOncePacketsAreSent(enet_uint32 data = 0);

		/**
		 * @brief Drops a peer entirely.
		 *
		 * Note: this function is not so different from disconnectImmediately().
		 */
		void drop();

		/**
		 * @brief Pings the host.
		 */
		void        ping() const;

		/**
		 * @brief Gets how long between every automatic ping between peer/host.
		 * @return The interval between pings.
		 */
		time::ms    getPingInterval() const;

		/**
		 * @brief Sets how long between every automatic ping between peer/host.
		 * @param interval The interval at which to ping the host.
		 */
		void        setPingInterval(time::ms interval);

		/**
		 * @brief Gets how long it takes for data to go somewhere and come back.
		 * @return The round trip time, from the peer to the host to the peer.
		 */
		time::ms    getRoundTripTime() const;
		
		/**
		 * @brief Gets the ratio of packet loss.
		 * @return The ratio of packet loss.
		 */
		enet_uint32 getPacketLoss() const;

		/**
		 * @brief Waits to receive a packet from a peer.
		 * @param callback The callback to use when a packet is received.
		 *
		 * Note: this function is useful if you want to filter a peer to receive
		 * a packet from, useful in the cases of authentication or waiting for
		 * something.
		 */
		void receive(Callback callback) const;

		/**
		 * @brief Sends a packet to the peer.
		 * @param packet The packet to send to the peer.
		 * @param channel The channel to send it on.
		 */
		void send(Packet& packet, enet_uint8 channel = 0);
		void send(Packet&& packet, enet_uint8 channel = 0);

		/**
		 * @brief Gets the current throttle parameters. 
		 * @return The current throttle parameters.
		 *
		 * To understand more about how the throttle works:
		 * http://enet.bespin.org/group__peer.html#gab35807c848b6c88af12ce8476dffbc84
		 */
		Throttle getThrottle() const;
		
		/**
		 * @brief Sets new throttle parameters.
		 * @param throttle The new throttle parameters.
		 * 
		 * To understand more about how the throttle works:
		 * http://enet.bespin.org/group__peer.html#gab35807c848b6c88af12ce8476dffbc84
		 */
		void     setThrottle(const Throttle& throttle);

		/**
		 * @brief Gets the current timeout parameters.
		 * @return The current parameters for when a timeout event is issued.
		 *
		 * Note: The timeout is how long the host will wait before receiving
		 * confirmation on connection before issuing a disconnect event.
		 */
		Timeout getTimeout() const;

		/**
		 * @brief Sets new timeout parameters.
		 * @param timeout The new parameters for when a timeout event is issued.
		 *
		 * Note: The timeout is how long the host will wait before receiving
		 * confirmation on connection before issuing a disconnect event.
		 */
		void    setTimeout(const Timeout& timeout);

		/**
		 * @brief Gets the address of the peer.
		 * @return The address of the peer.
		 */
		const Address& getAddress() const;

		/**
		 * @brief The current state of the peer.
		 * @return Gets the current state of the peer.
		 */
		PeerStatus getState() const;

		/**
		 * @brief Gets the peer's unique ID.
		 * @return The unique ID allocated to the peer.
		 *
		 * Note: This ID will always be unique, regardless of previous peers -
		 * this can be used to uniquely identify a peer with a user or something
		 * similar.
		 */
		std::size_t getID() const { return std::size_t(m_peer->data); }

		operator ENetPeer*() const { return m_peer; }

	private:
		ENetPeer* m_peer;

		Host*   m_host;
		Address m_address;
	};
} // namespace phx::net
