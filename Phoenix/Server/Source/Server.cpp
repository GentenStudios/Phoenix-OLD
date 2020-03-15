// Copyright 2019 Genten Studios
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

#include <SDL.h>

using namespace phx::server;
using namespace phx;

void Server::run()
{
	LoggerConfig config;
	config.threaded     = true;
	config.logFile      = "server.log";
	config.logToConsole = true;
	config.verbosity    = LogVerbosity::DEBUG;

	Logger::initialize(config);

	float       debugMessageTime   = 0.f;
	float       infoMessageTime    = 0.f;
	float       warningMessageTime = 0.f;
	float       errorMessageTime   = 0.f;
	std::size_t finish             = 0;

	std::size_t start = SDL_GetPerformanceCounter();
	for (int i = 0; i < 1000; ++i)
	{
		LOG_DEBUG("SERVER") << "This is the " << i << " debug message!";
	}
	finish           = SDL_GetPerformanceCounter();
	debugMessageTime = static_cast<float>(finish - start) /
	                   static_cast<float>(SDL_GetPerformanceFrequency());

	start = SDL_GetPerformanceCounter();
	for (int i = 0; i < 1000; ++i)
	{
		LOG_INFO("SERVER") << "This is the " << i << " info message!";
	}
	finish          = SDL_GetPerformanceCounter();
	infoMessageTime = static_cast<float>(finish - start) /
	                  static_cast<float>(SDL_GetPerformanceFrequency());

	start = SDL_GetPerformanceCounter();
	for (int i = 0; i < 1000; ++i)
	{
		LOG_WARNING("SERVER") << "This is the " << i << " warning message!";
	}
	finish             = SDL_GetPerformanceCounter();
	warningMessageTime = static_cast<float>(finish - start) /
	                     static_cast<float>(SDL_GetPerformanceFrequency());

	start = SDL_GetPerformanceCounter();
	for (int i = 0; i < 1000; ++i)
	{
		LOG_FATAL("SERVER") << "This is the " << i << " error message!";
	}
	finish           = SDL_GetPerformanceCounter();
	errorMessageTime = static_cast<float>(finish - start) /
	                   static_cast<float>(SDL_GetPerformanceFrequency());

	LOG_INFO("SERVER")
	    << "=============== BENCHMARK STATISTICS ===============";
	LOG_INFO("SERVER")
	    << "== Type    == Per 1000   == Per Message Average  ===";
	LOG_INFO("SERVER") << "== Debug   ==" << debugMessageTime
	                   << " == " << debugMessageTime / 1000.f;
	LOG_INFO("SERVER") << "== Info    ==" << infoMessageTime
	                   << " == " << infoMessageTime / 1000.f;
	LOG_INFO("SERVER") << "== Warning ==" << warningMessageTime
	                   << " == " << warningMessageTime / 1000.f;
	LOG_INFO("SERVER") << "== Error   ==" << errorMessageTime
	                   << " == " << errorMessageTime / 1000.f;

	Logger::teardown();
}
