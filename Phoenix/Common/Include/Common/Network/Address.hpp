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

#include <enet/enet.h>

#include <string>

namespace phx::net
{
	/**
	 * @brief Represents an address to a Host or Peer.
	 *
	 * This class provides an abstraction to set an Address and Port to aid
	 * connection through the Host class, as well as a way to identify a peer
	 * (not uniquely).
	 *
	 * @paragraph Usage
	 * @code
	 * // this is for the server, you set a Port to listen on, but not an
	 * // address to connect to, default address will be to listen on all
	 * // bindable network interfaces.
	 * Address address2(1234);
	 * Host server(address2);
	 *
	 * // address to connect to.
	 * Address address3("localhost", 1234);
	 * Host client;
	 * client.connect(address3);
	 * @endcode
	 */
	class Address
	{
	public:
		Address() = default;

		/**
		 * @brief Creates an address to bind to.
		 * @param port The port to listen on.
		 *
		 * The usage of this is mainly for something like a server application,
		 * where there is not an address to connect or listen to, except all
		 * bindable interfaces. Using this will allow a server to listen for
		 * data on a specific port - this is useless for a client, since you can
		 * just use the Host::connect method with the port and host address set
		 * correctly.
		 */
		explicit Address(enet_uint16 port);

		/**
		 * @brief Constructs an address to connect to.
		 * @param host The host that is going to be connected to.
		 * @param port The port that will be connected to.
		 *
		 * The usage of this constructor is more for the client side, where it
		 * connects to a peer, or rather a Server. This method is not the most
		 * ideal since it would require previous encoding of an IP or hostname
		 * to an unsigned integer. The other constructor requiring a string and
		 * a port are more suited to external use.
		 */
		Address(enet_uint32 host, enet_uint16 port);
		
		/**
		 * @brief Constructs a user-friendly address to connect to.
		 * @param host The hostname or IP that will be connected to.
		 * @param port The port that will be connected to.
		 *
		 * This constructor is made for the user end of the Client connection
		 * system. An IP such as 127.0.0.1 or 8.8.8.8 (Google) can be used, as
		 * well as a domain name, such as google.com or similar. The port has
		 * the same use. The pairing with Host::connect will allow a user to
		 * connect to any server they wish.
		 */
		Address(const std::string& host, enet_uint16 port);

		// copy constructor & assignment operator.
		Address(const ENetAddress& address);
		Address& operator=(const ENetAddress& address);

		// move constructor & assignment operator.
		Address(ENetAddress&& address);
		Address& operator=(ENetAddress&& address);

		/**
		 * @brief Sets the host to another value/address.
		 * @param host The hostname or IP that will be connected to.
		 *
		 * This should be preferred over the other setHost, with the signature
		 * requiring an unsigned integer. That is inconvenient for use and only
		 * exists for internal purposes.
		 */
		void setHost(const std::string& host);

		/**
		 * @brief Sets the host to another provided & pre-encoded value/address.
		 * @param host The host that will be connected to.
		 */
		void setHost(enet_uint32 host);

		/**
		 * @brief Sets the port to the provided value.
		 * @param port The port that will be connected to.
		 */
		void setPort(enet_uint16 port);

		/**
		 * @brief Gets the hostname that will be connected to.
		 * @return The hostname of the address that will be connected to.
		 */
		std::string getHostname() const;

		/**
		 * @brief Gets the IP that will be connected to.
		 * @return The IP of the address that will be connected to.
		 */
		std::string getIP() const;

		/**
		 * @brief Gets the port that will be connected to.
		 * @return The port that will be connected to.
		 */
		enet_uint16 getPort() const;

		operator const ENetAddress*() const { return &m_address; }

	private:
		ENetAddress m_address;
	};
} // namespace phx::net
