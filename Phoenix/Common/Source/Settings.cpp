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

#include <Common/Settings.hpp>

#include <climits>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <string>

using namespace phx;

Settings* Settings::instance()
{
	static Settings s_instance;
	return &s_instance;
}

void Settings::registerAPI(cms::ModManager* manager)
{
	// this lua integration only supports int, float, bool and strings right
	// now.

#define PHX_LUA_OVERLOAD(type)                                              \
	[](const std::string& key, type defaultValue) {                         \
		if (Settings::instance()->valid<type>(key))                         \
		{                                                                   \
			return static_cast<type>(                                       \
			    Settings::instance()->get<type>(key, true));                \
		}                                                                   \
		if (!Settings::instance()->exists(key))                             \
		{                                                                   \
			return static_cast<type>(Settings::instance()->get<type>(key) = \
			                             defaultValue);                     \
		}                                                                   \
		return defaultValue;                                                \
	}

	// we manually define the string one because we pass defaultValue as a const
	// reference.
	manager->registerFunction(
	    "core.settings.get",
	    sol::overload(
	        PHX_LUA_OVERLOAD(double), PHX_LUA_OVERLOAD(bool),
	        [](const std::string& key, const std::string& defaultValue) {
		        if (Settings::instance()->valid<std::string>(key))
		        {
			        return static_cast<std::string>(
			            Settings::instance()->get<std::string>(key, true));
		        }

		        if (!Settings::instance()->exists(key))
		        {
			        // get or will create an object since we know it doesn't
			        // exist.
			        // not super performant since the "exists" gets called twice
			        // but since this is ideally done at the start of a game
			        // being loaded, it should be fine.
			        Settings::instance()->get<std::string>(key) = defaultValue;
		        }

		        return defaultValue;
	        }));

#undef PHX_LUA_OVERLOAD
#define PHX_LUA_OVERLOAD(type)                                                 \
	[](const std::string& key, type value) {                                   \
		if (Settings::instance()->valid<type>(key))                            \
		{                                                                      \
			Settings::instance()->get<type>(key, true) = value;                \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			if (!Settings::instance()->exists(key))                            \
			{                                                                  \
				Settings::instance()->get<type>(key) = value;                  \
			}                                                                  \
                                                                               \
			LOG_FATAL("MODDING") << "A mod is attempting to store an invalid " \
			                        "value into a setting.";                   \
		}                                                                      \
	}

	manager->registerFunction(
	    "core.settings.set",
	    sol::overload(
	        PHX_LUA_OVERLOAD(double), PHX_LUA_OVERLOAD(bool),
	        [](const std::string& key, const std::string& value) {
		        if (Settings::instance()->valid<std::string>(key))
		        {
			        Settings::instance()->get<std::string>(key, true) = value;
		        }
		        else
		        {
			        if (!Settings::instance()->exists(key))
			        {
				        Settings::instance()->get<std::string>(key) = value;
			        }

			        LOG_FATAL("MODDING")
			            << "A mod is attempting to store an invalid "
			               "value into a setting.";
		        }
	        }));
}

bool Settings::parse(const std::string& configFile)
{
	m_configFilePath = configFile;

	std::string data;

	{
		namespace fs = std::filesystem;
		
		std::ifstream file {m_configFilePath};
		if (!file.is_open() && fs::exists(m_configFilePath))
		{
			// if it exists, but isn't open - something clearly went wrong.
			// if it isn't open but doesn't exist, it's not a problem.
			return false;
		}

		file.seekg(0, std::ios::end);
		data.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&data[0], data.size());
	}

	m_settings = nlohmann::json::parse(data, nullptr, false);

	// is_discarded returns false if the parsing failed.
	if (m_settings.is_discarded())
	{
		return false;
	}

	return true;
}

void Settings::save()
{
	std::ofstream file {m_configFilePath};
	if (!file.is_open())
	{
		// since ofstream will create a file, if the file is not open, there is
		// another error - such as permissions.

		// if logger is initialized.
		if (Logger::get() != nullptr)
		{
			LOG_FATAL("SETTINGS") << "Could not open settings file to save, no "
			                         "new/overridden settings will be written.";
		}
		else
		{
			// logger not initialized, output to std::cerr.
			std::cerr << "[FATAL]"
			          << " Could not open settings file to save to. No "
			             "new/overridden settings will be written."
			          << std::endl;
		}
		
		return;
	}
	
	file << std::setw(4) << m_settings << std::endl;
}

void Settings::changeSavePath(const std::string& newConfig)
{
	m_configFilePath = newConfig;
}

bool Settings::exists(const std::string& key) const
{
	return m_settings.find(key) != m_settings.end();
}

std::vector<Settings::KeyValPair> Settings::getImplFinalSettings()
{
	std::vector<KeyValPair> finalSettings;

	for (auto& [key, val] : m_settings.items())
	{
		if (m_invalidOverwriter.find(key) == m_invalidOverwriter.end())
		{
			finalSettings.push_back({key, &val});
			continue;
		}

		finalSettings.push_back({key, &m_invalidOverwriter[key]});
	}

	return finalSettings;
}
