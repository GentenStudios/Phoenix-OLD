// Copyright 2020 Genten Studios
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
#include <Common/Settings.hpp>

#include <iostream>
#include <thread>
#include <utility>

using namespace phx::server;
using namespace phx;

Server::Server(std::string save) : m_save(std::move(save))
{
	m_iris = new networking::Iris(&m_registry, &m_running);
	m_game = new Game(&m_registry, &m_running, m_iris);
}

void Server::run()
{
	std::cout << "Hello, Server!" << std::endl;
	Settings::get()->load("config.txt");
	m_running = true;

	std::thread t_iris(&networking::Iris::run, m_iris);
	std::thread t_game(&Game::run, m_game);

	std::string input;
	while (m_running)
	{
		/// @todo Replace simple loop with commander
		std::cin >> input;
		if (input == "q")
		{
			m_running = false;
		}
	}
	t_iris.join();
	t_game.join();
	Settings::get()->save("config.txt");
}

Server::~Server()
{ delete m_iris; }