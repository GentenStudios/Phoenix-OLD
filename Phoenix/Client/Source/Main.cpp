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

#include <Client/Client.hpp>

#include <Common/Logger.hpp>
#include <Common/Network/Host.hpp>
#include <Common/Network/Packet.hpp>

static std::vector<std::byte> pack(const std::string& string)
{
	std::vector<std::byte> arr(string.length() + 1);
	std::transform(string.begin(), string.end(), arr.begin(),
	               [](char c) { return std::byte(c); });

	return std::move(arr);
}

static std::string unpack(const std::vector<std::byte>& data)
{
	std::string string;
	string.resize(data.size() + 1);
	std::transform(data.begin(), data.end(), string.begin(),
	               [](std::byte c) { return char(c); });

	return string;
}

using namespace phx;

#undef main
int main(int argc, char** argv)
{
	LoggerConfig config;
	config.verbosity = LogVerbosity::DEBUG;

	Logger::initialize(config);

	// client::Client::get()->run();

	auto clientCode = [](std::string username) {
		bool running = true;

		net::Host client;
		client.onConnect([username](net::Peer& peer, enet_uint32 data) {
			LOG_INFO("CLIENT") << "Connected to server.";
			peer.ping();
			LOG_INFO("CLIENT")
			    << username
			    << " has a ping of: " << peer.getRoundTripTime().count();

			char* name = new char[strlen("server") + 1];
			memcpy(name, "server", strlen("server"));
			name[strlen("server")] = '\n';
			peer.setData(name);
		});

		client.onDisconnect([username, &running](void* data, enet_uint32) {
			if (std::string(static_cast<char*>(data)) == std::string("server"))
			{
				running = false;
				LOG_INFO("CLIENT") << "Server timed out.";
			}
		});

		client.onReceive(
		    [](net::Peer&, net::Packet&& packet, enet_uint8 channel) {
			    LOG_INFO("CLIENT") << unpack(packet.getData());
		    });

		net::Address serverAddr;
		serverAddr.setHost("127.0.0.1");
		serverAddr.setPort(7777);

		auto serverOptional = client.connect(serverAddr);

		if (!serverOptional)
		{
			LOG_FATAL("CLIENT") << "Could not connect to server!";
			return;
		}

		auto& peer = serverOptional.value().get();
		peer.setTimeout({10000_ms, 0_ms, 10000_ms});

		peer.send(net::Packet(pack("password;password;"),
		                      net::PacketFlags::RELIABLE));
		client.flush();

		while (running)
		{
			client.poll(10);
		}
	};

	clientCode("toby");

	return 0;
}
