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

#include <Server/Server.hpp>

#include <Common/Logger.hpp>
#include <Common/Network/Host.hpp>
#include <Common/Network/Packet.hpp>

#include <algorithm>
#include <string>
#include <thread>

using namespace phx;

static std::vector<std::byte> pack(const std::string& string)
{
	std::vector<std::byte> arr(string.length());
	std::transform(string.begin(), string.end(), arr.begin(),
	               [](char c) { return std::byte(c); });

	return std::move(arr);
}

static std::string unpack(const std::vector<std::byte>& data)
{
	std::string string;
	string.resize(data.size());
	std::transform(data.begin(), data.end(), string.begin(),
	               [](std::byte c) { return char(c); });

	return string;
}

std::string descClient(const net::Peer& peer)
{
	auto addr = peer.getAddress();
	auto name = "[" + addr.getIP() + ":" + std::to_string(addr.getPort()) + "]";
	return name + " # " + std::to_string(peer.getID());
}

#undef main
int main(int argc, char** argv)
{
	LoggerConfig config;
	config.verbosity = LogVerbosity::DEBUG;

	Logger::initialize(config);

	const auto port = 1234u;

	net::Host server(net::Address {port}, 32);

	std::vector<std::size_t> users;

	server.onConnect([&users](net::Peer& peer, enet_uint32) {
		LOG_INFO("SERVER") << "New connection: " << descClient(peer);
		peer.send({pack("yoyo!"), net::PacketFlags::RELIABLE});
		users.push_back(peer.getID());
	});

	server.onDisconnect([&users](std::size_t id, enet_uint32) {
		LOG_INFO("SERVER") << "A client disconnected.";
		auto it = std::find(users.begin(), users.end(), id);
		if (it != users.end())
		{
			users.erase(it);
		}
	});

	bool work = true;
	server.onReceive(
	    [&work, &server, &users](net::Peer& peer, net::Packet&& packet, enet_uint32) {
		    auto data = unpack(packet.getData());
		    LOG_INFO("SERVER") << "New packet from: " << descClient(peer)
		                       << "\n\tContents: " << data;
		    peer.send({pack("quit"), net::PacketFlags::RELIABLE});

		    if (data == "quit")
		    {
			    peer.disconnectImmediately();
		    }
	    });

	while (work)
	{
		server.poll(2000_ms);
		LOG_INFO("PLAYERS") << users.size() << " players connected";
		for (auto user : users)
		{
			LOG_DEBUG("PLAYERS") << user << " is connected.";
		}
	}

	Logger::teardown();

	return 0;
}
