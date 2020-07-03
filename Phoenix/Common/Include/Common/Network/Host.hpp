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
#include <unordered_map>

namespace phx::net
{
	/**
	 * @brief Represents a host, acting as either a server or a client.
	 *
	 * This class provides the functionality for an application to receive,
	 * connect and send data to peers.
	 *
	 * The terminology of "channels" is a system where data can be separated
	 * when sent and received. This allows for data to be handled separately,
	 * removing the need for space-wasting headers - this is handled internally.
	 *
	 * A note to keep in mind is that the maximum number of peers possible is
	 * 4096.
	 *
	 * @paragraph Usage
	 * The constructor requiring only the amount of peers and address has been
	 * setup so the defaults are correct for a client. The only requires a
	 * single peer (the server) and the address is blank since it will listen to
	 * all binding interfaces on the computer, for replies from the server.
	 *
	 * The other constructor is more ideal for a server, where an address -
	 * which can be set so only the port is defined and all bindable interfaces
	 * are listened to - and the amount of peers, alongside the number of
	 * required channels is provided.
	 *
	 * @code
	 * Host client;
	 *
	 * // set the server to listen on port 1234.
	 * Host server(Address{1234});
	 *
	 * Address serverAddress = Address("127.0.0.1", 1234);
	 * client.connect(serverAddress);
	 *
	 * server.onConnect([](Peer& peer, enet_uint32) { std::cout << "Client
	 *					connected from: " << peer.getAddress().getIP(); });
	 *
	 * server.onReceive([](Peer& peer, Packet&&, enet_uint32) { std::cout <<
	 *				"Received packet from: " << peer.getAddress().getIP(); });
	 *
	 * server.onDisconnect([](std::size_t peerID, enet_uint32) { std::cout <<
	 *					"Client disconnected: " << peerID; });
	 *
	 * client.onConnect([](Peer&, enet_uint32) { std::cout << "connected to
	 *					server"; });
	 *
	 * client.onReceive([](Peer&, Packet&&, enet_uint8) { std::cout <<
	 *					"received packet from server."; });
	 *
	 * client.onDisconnect([](std::size_t peerID, enet_uint32) { std::cout <<
	 *					"Disconnected from server"; });
	 *
	 * thread 1:
	 * while (running) server.poll();
	 *
	 * thread 2:
	 * while (running) client.poll();
	 * @endcode
	 */
	class Host
	{
	public:
		using ReceiveCallback =
		    std::function<void(Peer&, Packet&&, enet_uint32)>;
		using ConnectCallback = std::function<void(Peer&, enet_uint32)>;
		using DisconnectCallback =
		    std::function<void(std::size_t, enet_uint32)>;

	public:
		/**
		 * @brief Creates a Host with the correct default values for a Client.
		 * @param peers The amount of peers to allow connections from.
		 * @param address The address to bind to.
		 *
		 * This constructor is ideal for the client - the defaults provided
		 * allow for 1 peer (the server).
		 */
		Host(std::size_t peers = 1, const ENetAddress* address = nullptr);

		/**
		 * @brief Creates a Host more suited to a server application.
		 * @param address The address to bind to - for listening on.
		 * @param peers The maximum amount of peers that are allowed to connect.
		 * @param channels The maximum number of channels that can be used.
		 *
		 * The address can be an address constructed with only a port. This
		 * allows for listening on the port on of all bindable interfaces.
		 * Providing a host/IP will allow you to listen on a specific IP if for
		 * example, a server has multiple IPs provided to it.
		 */
		Host(const Address& address, std::size_t peers,
		     std::size_t channels = 0);

		~Host();

		// check if value exists before using.
		using OptionalPeer = std::optional<std::reference_wrapper<Peer>>;

		/**
		 * @brief Connects to a provided address.
		 * @param address The address to connect to.
		 * @return A peer inside an std::optional in case the connection fails.
		 *
		 * @paragraph Usage
		 * You must call the Host::poll function once a connection request has
		 * been created. The connection data will not be dispatched until poll
		 * has been called, a recommended example of connecting is shown below:
		 * @code
		 * bool connected;
		 * client.onConnect([&connected](Peer&, enet_uint32) { connected = true;
		 *		});
		 *
		 * Peer server;
		 *
		 * auto peer = client.connect(Address{ "127.0.0.1", 1234 });
		 * if (peer)
		 * {
		 *	   // value for the optional, get for the reference_wrapper
		 *     server = peer.value().get();
		 * }
		 *
		 * // poll for 5 seconds.
		 * client.poll(5000_ms);
		 *
		 * if (connected == false)
		 * {
		 *     // connection failed, try in a loop a few times if you want, but
		 *     // it's already been tried.
		 * }
		 * @endcode
		 */
		OptionalPeer connect(const Address& address);

		/**
		 * @brief Connects to a provided address.
		 * @param address The address to connect to.
		 * @param channels The amount of channels to enable on connect.
		 * @param data User data to send, use only if understood.
		 * @return A peer inside an std::optional in case the connection fails.
		 */
		OptionalPeer connect(const Address& address, enet_uint8 channels,
		                     enet_uint32 data = 0);

		/**
		 * @brief Gets the current bandwidth limits (incoming, outgoing).
		 * @return The current bandwidth limits, in bytes per second.
		 */
		Bandwidth getBandwidthLimit() const;

		/**
		 * @brief Sets a new bandwidth limit.
		 * @param bandwidth The bandwidth limits (incoming, outgoing) in bytes
		 * per second.
		 */
		void setBandwidthLimit(const Bandwidth& bandwidth);

		
		/**
		 * @brief Gets the number of currently allocated channels.
		 * @return The current number of allocated channels.
		 */
		std::size_t getChannelLimit() const;

		/**
		 * @brief Sets a new limit for the allocated channels.
		 * @param limit A new limit for number of allocated channels.
		 */
		void setChannelLimit(std::size_t limit);

		/**
		 * @brief Broadcasts a packet to all connected peers.
		 * @param packet The packet to send to all peers.
		 * @param channel The channel to send it on.
		 */
		void broadcast(Packet& packet, enet_uint8 channel = 0);
		void broadcast(Packet&& packet, enet_uint8 channel = 0);

		/**
		 * @brief Sets a callback for when a packet is received.
		 * @param callback The function to call when a packet is received.
		 */
		void onReceive(ReceiveCallback callback);

		/**
		 * @brief Sets a callback for when a new connection is established.
		 * @param callback The function to call when a new peer is connected.
		 *
		 * Note: You don't get notified for your outgoing connections, like
		 * clients to server, but when the server accepts the connection, you
		 * get notified of a "request" - which is why you have to poll for
		 * network events once a request has been created.
		 */
		void onConnect(ConnectCallback callback);

		/**
		 * @brief Sets a callback for when a peer is disconnected.
		 * @param callback The function to call when a peer is disconnected.
		 *
		 * Note: You do get notified when a peer times out, etc...
		 */
		void onDisconnect(DisconnectCallback callback);

		/**
		 * @brief Polls for any network events that have occurred.
		 * @param limit The limit of events that should be processed in this
		 * call.
		 *
		 * Note: This function will not wait for events to occur, if there is
		 * nothing in the queue immediately, this function will just end.
		 */
		void poll(int limit = 1);
		
		/**
		 * @brief Polls & waits for any network events that have occurred.
		 * @param timeout How long to wait for an event for.
		 * @param limit The maximum amount of events that should be processed.
		 */
		void poll(time::ms timeout, int limit = 1);

		/**
		 * @brief Flushes all events, sending them off to the respective foreign
		 * hosts.
		 */
		void flush();

		/**
		 * @brief Gets the number of connected peers.
		 * @return The number of connected peers.
		 */
		std::size_t getPeerCount() const;

		/**
		 * @brief Gets the maximum number of possible connected peers.
		 * @return The maximum amount of peers that there can be.
		 */
		std::size_t getPeerLimit() const;

		/**
		 * @brief Gets the address of this host.
		 * @return The address of this host.
		 *
		 * This is for the most part, fairly useless, but it can be used to get
		 * what port is being used if that data is lost down the line.
		 */
		const Address& getAddress() const;

		/**
		 * @brief Gets a peer based on its ID.
		 * @param id The ID of the Peer.
		 * @return A pointer to the peer, or a nullptr if it doesn't exit.
		 */
		Peer* getPeer(std::size_t id);

		/**
		 * @brief Gets the total amount of data received.
		 * @return The total amount of data received in bytes.
		 *
		 * Note: This value can overflow. Don't use if not needed.
		 */
		enet_uint32 getTotalReceievedData() const;

		/**
		 * @brief Gets the total amount of data sent.
		 * @return The total amount of data sent in bytes.
		 *
		 * Note: This value can overflow. Don't use if not needed.
		 */
		enet_uint32 getTotalSentData() const;

		operator ENetHost*() const { return m_host; }

	private:
		void handleEvent(ENetEvent& event);

		Peer& getPeer(ENetPeer& peer);
		Peer& createPeer(ENetPeer& peer);
		void  removePeer(ENetPeer& peer);

		friend class Peer;
		void disconnectPeer(std::size_t id);
		void removePeer(const Peer& peer);

	private:
		ENetHost* m_host;
		Address   m_address;

		ReceiveCallback    m_receiveCallback;
		ConnectCallback    m_connectCallback;
		DisconnectCallback m_disconnectCallback;

		std::size_t                           m_peerID = 0;
		std::unordered_map<std::size_t, Peer> m_peers;

		static std::atomic<std::size_t> m_activeInstances;
	};
} // namespace phx::net
