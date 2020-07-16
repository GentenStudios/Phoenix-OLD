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

#include <Common/Save.hpp>
#include <Common/Logger.hpp>

#include <filesystem>
#include <iomanip>
#include <fstream>

using namespace phx;

Save::Save(const std::string& save)
{
	namespace fs = std::filesystem;

	// check if the save exists
	auto path = fs::current_path() / "Saves" / save;
	if (!fs::exists(path))
	{
		LOG_FATAL("SAVES") << "Save could not be located, quitting.";
		exit(EXIT_FAILURE);
	}

	// save exists, lets load what we need.
	std::ifstream json(path.append(save + ".json"));
	if (!json.is_open())
	{
		LOG_FATAL("SAVES") << "The save: " << save
		                   << " does not have a valid configuration, quitting.";
		exit(EXIT_FAILURE);
	}

	json >> m_data.settings;
	if (!m_data.settings["mods"].is_array())
	{
		LOG_FATAL("SAVES") << "The save: " << save
		                   << " does not have a valid configuration, quitting.";
		exit(EXIT_FAILURE);
	}

	m_data.mods = m_data.settings["mods"].get<std::vector<std::string>>();
	m_data.name = m_data.settings["name"].get<std::string>();

	LOG_INFO("SAVES") << "The save: " << m_data.name << " requires the mods: ";
	for (const std::string& mod : m_data.mods)
	{
		LOG_INFO("SAVES") << mod;
	}
}

Save::~Save()
{
}

Save Save::createSave(const SaveConfig& config)
{
	namespace fs = std::filesystem;

	auto path = fs::current_path() / "Saves" / config.name;
	if (fs::exists(path))
	{
		LOG_WARNING("SAVES") << "Save already exists, loading existing save.";
		return Save(config.name);
	}

	// save doesn't already exist, lets make it.
	fs::create_directory(path);

	// we make a new json thing so we don't double the size of the config struct.
	nlohmann::json saveSettings;
	
	saveSettings["name"] = config.name;
	saveSettings["mods"] = config.mods;
	saveSettings += config.settings;

	std::ofstream json(path.append(config.name + ".json"));
	json << std::setw(4) << saveSettings;
	json.close();

	return Save(config.name);
}

const std::string& Save::getName() const { return m_data.name; }
