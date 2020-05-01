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
#include <Common/Network/Address.hpp>

using namespace phx::net;

Address::Address(enet_uint16 port) : Address(ENET_HOST_ANY, port) {}

Address::Address(enet_uint32 host, enet_uint16 port)
{
	m_address.host = host;
	m_address.port = port;
}

Address::Address(const std::string& host, enet_uint16 port)
{
	setHost(host);
	m_address.port = port;
}

Address::Address(const ENetAddress& address) { m_address = address; }
Address& Address::operator=(const ENetAddress& address)
{
	m_address = address;
	return *this;
}

Address::Address(ENetAddress&& address) { m_address = address; }
Address& Address::operator=(ENetAddress&& address)
{
	m_address = address;
	return *this;
}

void Address::setHost(const std::string& host)
{
	if (enet_address_set_host(&m_address, host.c_str()))
	{
		LOG_FATAL("NETCODE") << "Could not resolve hostname.";
	}
}

void Address::setHost(enet_uint32 host) { m_address.host = host; }

void Address::setPort(enet_uint16 port) { m_address.port = port; }

std::string Address::getHostname() const
{
	// hostnames can be max of 255 bytes.
	constexpr int MaxHostNameBytes = 255;

	// + 1 for null terminator.
	char hostname[MaxHostNameBytes + 1];
	if (enet_address_get_host(&m_address, hostname, MaxHostNameBytes))
	{
		LOG_FATAL("NETCODE") << "Could not resolve hostname.";
	}

	return hostname;
}

std::string Address::getIP() const
{
	// enet doesn't support ipv6 afaik.
	constexpr int IPv4MaxBytes = 15;

	// + 1 for null terminator.
	char ip[IPv4MaxBytes + 1];
	if (enet_address_get_host_ip(&m_address, ip, IPv4MaxBytes))
	{
		LOG_FATAL("NETCODE") << "Failed to get IP.";
	}

	return ip;
}

enet_uint16 Address::getPort() const { return m_address.port; }
