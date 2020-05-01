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
	std::vector<std::byte> arr(string.length() + 1);
	std::transform(string.begin(), string.end(), arr.begin(),
	               [](char c) { return std::byte(c); });

	return std::move(arr);
}

static std::string unpack(const std::vector<std::byte>& data)
{
	std::string string;
	std::transform(data.begin(), data.end(), string.begin(),
	               [](std::byte c) { return char(c); });

	return string;
}

#undef main
int main(int argc, char** argv)
{
	LoggerConfig config;
	config.verbosity = LogVerbosity::DEBUG;

	Logger::initialize(config);

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

		peer.send(net::Packet(pack("password;password;" + username),
		                      net::PacketFlags::RELIABLE), 0);

		peer.setTimeout({10000_ms, 0_ms, 10000_ms});

		client.flush();

		while (running)
		{
			client.poll(10);
		}
	};
	
	auto work = []()
	{
		net::Host server(net::Address(7777), 10);

		bool running = true;

		server.onConnect([&server](net::Peer& peer, enet_uint32 data) {
			LOG_INFO("SERVER") << "Player attempting connection...";
			peer.send(net::Packet(pack("yoyo!"), net::PacketFlags::RELIABLE));
			server.flush();
		});

		server.onReceive([&running](net::Peer& peer, net::Packet&& packet,
		                            enet_uint8 channel) {
			LOG_INFO("SERVER") << "Receiving packet!";
			if (channel == 0)
			{
				std::string data = unpack(packet.getData());

				std::vector<std::string> parts;

				std::stringstream stream(data);
				std::string       input;
				while (std::getline(stream, input, ';'))
				{
					parts.push_back(input);
				}

				if (parts[0] == parts[1])
				{
					LOG_INFO("SERVER")
					    << "Player: " << parts[2] << " has connected!";

					char* name = new char[parts[3].length() + 1];
					memcpy(name, parts[3].c_str(), parts[3].length());
					name[parts[3].length()] = '\0';

					LOG_DEBUG("SERVER") << name;

					peer.setData(name);
				}
			}
			else if (channel == 1)
			{
				std::string data = unpack(packet.getData());
				if (data == "quit")
				{
					running = false;
				}
			}
		});

		server.onDisconnect([](void* data, enet_uint32) {
			const char* name = static_cast<char*>(data);
			LOG_INFO("SERVER") << "Player: " << name << " has disconnected";
			delete static_cast<char*>(data);
		});

		
		while (running)
		{
			server.poll(10);
		}
	};

	work();
	
	Logger::teardown();

	return 0;
}
