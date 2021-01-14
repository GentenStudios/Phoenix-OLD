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
#include <iomanip>
#include <iostream>

using namespace phx;

Settings* Settings::instance()
{
	static Settings s_instance;
	return &s_instance;
}

bool Settings::parse(const std::string& configFile)
{
	m_configFilePath = configFile;

	std::string data;

	{
		std::ifstream file {m_configFilePath};
		if (!file.is_open())
		{
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
	file << std::setw(4) << m_settings << std::endl;
}

void Settings::saveTo(const std::string& newConfig)
{
	m_configFilePath = newConfig;
}

bool Settings::exists(const std::string& key) const
{
	return m_settings.find(key) != m_settings.end();
}
