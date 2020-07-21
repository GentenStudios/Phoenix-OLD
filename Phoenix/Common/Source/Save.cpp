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

#include <Common/Logger.hpp>
#include <Common/Save.hpp>

#include <filesystem>
#include <fstream>
#include <iomanip>

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

	nlohmann::json saveSettings;
	
	json >> saveSettings;
	if (!saveSettings["mods"].is_array())
	{
		LOG_FATAL("SAVES") << "The save: " << save
		                   << " does not have a valid configuration, quitting.";
		exit(EXIT_FAILURE);
	}

	m_data.mods     = saveSettings["mods"].get<std::vector<std::string>>();
	m_data.name     = saveSettings["name"].get<std::string>();
	m_data.settings = saveSettings["settings"].get<nlohmann::json>();
}

Save::~Save()
{
	// save on destruct.
	toFile();
}

Save Save::createSave(const SaveConfig& config)
{
	namespace fs = std::filesystem;

	const auto path = fs::current_path() / "Saves" / config.name;
	if (fs::exists(path))
	{
		LOG_WARNING("SAVES") << "Save already exists, loading existing save.";
		return Save(config.name);
	}

	// save doesn't already exist, lets make it.
	fs::create_directory(path);

	// we make a new json thing so we don't double the size of the config
	// struct.
	nlohmann::json saveSettings;

	saveSettings["name"] = config.name;
	saveSettings["mods"] = config.mods;
	saveSettings["settings"] = config.settings;

	std::ofstream json(path / (config.name + ".json"));
	json << std::setw(4) << saveSettings;
	json.close();

	return Save(config.name);
}

const std::string& Save::getName() const { return m_data.name; }

void Save::toFile(const std::string& name)
{
	namespace fs = std::filesystem;

	// if the name is not empty or the name of the current save, choose to make
	// a new save.
	if (!name.empty() && name != m_data.name)
	{
		m_data.name = name;

		const auto path = fs::current_path() / "Saves" / name;

		// save already exists, overwrite json.
		if (fs::exists(path))
		{
			LOG_WARNING("SAVES")
			    << "Target folder for save renaming operation already exists, "
			       "proceeding by overwriting folder contents.";

			/// @todo implement system to potentially backup the existing save
			// empty the folder of the existing save.
			fs::remove_all(path);

			// write new json file.
			nlohmann::json saveSettings;
			saveSettings["name"] = name;
			saveSettings["mods"] = m_data.mods;
			saveSettings["settings"] = m_data.settings;

			std::ofstream json(path / (name + ".json"));
			json << std::setw(4) << saveSettings;
			json.close();
		}
		else
		{
			// folder doesn't already exist, lets make it.
			fs::create_directory(path);

			// write new json file.
			nlohmann::json saveSettings;
			saveSettings["name"]     = m_data.name;
			saveSettings["mods"]     = m_data.mods;
			saveSettings["settings"] = m_data.settings;

			std::ofstream json(path / (m_data.name + ".json"));
			json << std::setw(4) << saveSettings;
			json.close();
		}
	}
	else
	{
		// the save hasn't been renamed, and the settings have changed, now
		// update file, otherwise there's no point.

		const auto path = fs::current_path() / "Saves" / m_data.name;

		// this exists as a error check to prevent an exception being thrown when opening the json file.
		if (!fs::exists(path))
		{
			LOG_WARNING("SAVES")
			    << "The save that was originally opened no longer exists, it "
			       "will be recreated, however there may be data loss.";

			fs::create_directory(path);

			// setting this will make sure that the settings file is created in the below if statement.
			m_settingsChanged = true;
		}
		
		if (m_settingsChanged)
		{
			nlohmann::json saveSettings;
			saveSettings["name"] = name;
			saveSettings["mods"] = m_data.mods;
			saveSettings["settings"] = m_data.settings;

			std::ofstream writeSettings(path);
			writeSettings << std::setw(4) << saveSettings;
			writeSettings.close();
		}
	}

	// whichever pathway is chosen, the settings are updated, or not if not
	// changed, so lets just force this to false.
	m_settingsChanged = false;

	// we've already altered all the JSON's and paths, now save all dimensions
	// (etc...). dimensions will need a function like this where the
	// save/dimension name is "changeable".
}
