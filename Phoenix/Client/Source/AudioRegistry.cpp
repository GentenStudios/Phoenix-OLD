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

#include <Client/AudioRegistry.hpp>

using namespace phx::client;

void AudioRegistry::registerAPI(cms::ModManager* manager,
                                SoLoud::Soloud*  soloud)
{
	manager->registerFunction(
	    "core.audio.register", [manager, this](sol::table source) {
		    sol::optional<std::string> id = source["id"];
		    if (!id)
		    {
			    // log the error and return to make this a recoverable
			    // error.
			    LOG_FATAL("MODDING")
			        << "The mod at: " << manager->getCurrentModPath()
			        << " attempted to register an audio source without "
			        << "specifying an id.";
			    return;
		    }
		    sol::optional<std::string> path = source["path"];
		    if (!path)
		    {
			    // log the error and return to make this a recoverable
			    // error.
			    LOG_FATAL("MODDING")
			        << "The mod at: " << manager->getCurrentModPath()
			        << " attempted to register an audio source without "
			        << "specifying a source path.";
			    return;
		    }
		    add(manager->getCurrentModPath() + *path, *id);
	    });

	manager->registerFunction("core.audio.play",
	                          [soloud, this](const std::string& source) {
		                          soloud->play(*getByID(source));
	                          });
}

std::size_t AudioRegistry::add(const std::string& path, const std::string& id)
{

	std::size_t next = sources.size();
	sources.add(next, SoLoud::Wav());
	referrer.add(id, next);
	SoLoud::Wav* source = sources.get(next);
	if (source->load(path.c_str()) != 0)
	{
		LOG_FATAL("AUDIO") << "Failed to load sound file: " << path;
	}
	LOG_INFO("AUDIO") << "Source registered";
	return next;
}