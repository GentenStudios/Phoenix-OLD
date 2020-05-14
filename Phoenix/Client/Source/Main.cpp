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
#include <Client/Graphics/GUI/Container.hpp>

#include <Common/Logger.hpp>
#include <iostream>

#include <glad/glad.h>

using namespace phx;

#undef main
int main(int argc, char** argv)
{
	std::cout << "Um ok?" << std::endl;
	
	LoggerConfig config;
	config.logToConsole = true;
	config.threaded     = false;
	config.verbosity    = LogVerbosity::DEBUG;

	Logger::initialize(config);

	LOG_INFO("What the fuck?") << "okely dokely do";
	
	// client::Client::get()->run();

	gfx::Window window("okely dokely", 1280, 720);

	// positioning is done on a scale of 0 to 100.
	gui::Container container("", {50, 50}, {100, 100}, {255, 0, 255}, 1.f, &window,
	                         gui::Container::Flags::COLLAPSIBLE);

	auto rect = new gui::Rectangle(&container, {50, 50}, {100, 100},
	                               {255, 128, 255}, 1.f);

	// position.y is 70, because size is in each direction, so for the previous
	// rectangle, it's 50 + size.y which is 60. And, because it's still 10 in
	// each direction for this rectangle, you add 10 that to get 70.
	auto rect2 = new gui::Rectangle(&container, {50, 100}, {50, 10},
                               {255, 0, 255}, 1.f);

	
	while (window.isRunning())
	{
		window.startFrame();

		// dummy dt.
		container.tick(16.6f);
		//rect->tick(16.6);
		
		window.endFrame();
	}

	delete rect;

	return 0;
}
