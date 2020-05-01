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
	std::vector<std::byte> arr(string.length());
	std::transform(string.begin(), string.end(), arr.begin(),
	               [](char c) { return std::byte(c); });

	return arr;
}

static std::string unpack(const std::vector<std::byte>& data)
{
	std::string string;
	string.resize(data.size());
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

	const auto hostname = "127.0.0.1";
	const auto port     = 1234u;

	net::Host client;

	client.onConnect(
	    [](net::Peer&, enet_uint32) { LOG_INFO("CLIENT") << "Connected."; });
	client.onDisconnect([](std::size_t, enet_uint32) {
		LOG_INFO("CLIENT") << "Disconnected.";
	});

	bool work = true;
	client.onReceive([&work](net::Peer&, net::Packet&& packet, enet_uint32) {
		auto data = unpack(packet.getData());
		if (data == "quit")
		{
			work = false;
			LOG_DEBUG("CLIENT") << "Shutting down.";
		}

		LOG_INFO("CLIENT") << "Server says: (" << data.size() << ") " << data;
		std::cout << ">> ";
	});

	auto server = client.connect({hostname, port}).value().get();

	std::thread t1([&work, &client]() {
		while (work)
		{
			client.poll(100_ms);
		}
	});

	while (work)
	{
		std::string input;
		std::cout << ">> ";
		std::getline(std::cin, input);

		auto data = pack(input);
		server.send({data, net::PacketFlags::RELIABLE}, 0);
	}

	t1.join();

	return 0;
}
