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

#include <Common/Voxels/BlockReferrer.hpp>

#include <Common/Logger.hpp>
#include <Common/Settings.hpp>

#include <iostream>
#include <thread>

using namespace phx::server;
using namespace phx;

Server::Server(const std::string& save)
{
    // use this as a placeholder until we have command line arguments.
    // even if the list is empty, it can create/load a save as required.
    // listing mods but loading an existing save will NOT load more mods, you
    // must manually edit the JSON to load an another mod after initialization.
    std::vector<std::string> commandLineModList = {"mod1", "mod2", "mod3"};
	m_save = new Save(save, commandLineModList);
	m_iris = new server::net::Iris(&m_registry);
	m_game = new Game(&m_blockRegistry, &m_registry, m_iris, m_save);
}

void registerUnusedAPI(cms::ModManager* manager)
{
	manager->registerFunction("core.input.registerInput",
	                          [](std::string uniqueName,
	                             std::string displayName,
	                             std::string defaultKey) {});
	manager->registerFunction("core.input.getInput", [](int input) {});
	manager->registerFunction("core.input.getInputRef",
	                          [](std::string uniqueName) {});
	manager->registerFunction("core.input.registerCallback",
	                          [](int input, sol::function f) {});
	manager->registerFunction(
	    "audio.loadMP3",
	    [=](const std::string& uniqueName, const std::string& filePath) {});
	manager->registerFunction("audio.play", [=](sol::table source) {});
}

void Server::run()
{
	std::cout << "Hello, Server!" << std::endl;

	LoggerConfig config;
	config.verbosity = LogVerbosity::DEBUG;
	Logger::initialize(config);

	Settings::get()->load("config.txt");

	// Initialize the Modules //

	m_modManager = new cms::ModManager(m_save->getModList(), {"Modules"});

	m_modManager->registerFunction("core.print", [=](const std::string& text) {
		std::cout << text << "\n";
	});

	m_blockRegistry.registerAPI(m_modManager);
	Settings::get()->registerAPI(m_modManager);
	m_game->registerAPI(m_modManager);
	registerUnusedAPI(m_modManager);

	m_modManager->registerFunction("core.log_warning", [](std::string message) {
		LOG_WARNING("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_fatal", [](std::string message) {
		LOG_FATAL("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_info", [](std::string message) {
		LOG_INFO("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_debug", [](std::string message) {
		LOG_DEBUG("MODULE") << message;
	});

	float progress = 0.f;
	auto  result   = m_modManager->load(&progress);

	if (!result.ok)
	{
		LOG_FATAL("CMS") << "An error has occurred loading modules.";
		exit(EXIT_FAILURE);
	}

	// Modules Initialized //

	// Fire up Threads //

	std::thread t_iris(&server::net::Iris::run, m_iris);
	std::thread t_game(&Game::run, m_game);

	// Enter Main Loop //

    m_running = true;
	std::string input;
	while (m_running)
	{
		/// @todo Replace simple loop with commander
		std::cin >> input;
		if (input == "q")
		{
			m_running = false;
			m_iris->kill();
		}
	}

	// Begin Shutdown //

	t_iris.join();
	t_game.join();
	Settings::get()->save("config.txt");
}

Server::~Server()
{
	delete m_iris;
	delete m_game;
	delete m_modManager;
	delete m_save;
}
