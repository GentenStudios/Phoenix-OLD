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

Setting::Setting(std::string name, std::string key, int defaultValue,
                 json* json_)
    : m_name(std::move(name)), m_key(std::move(key)), m_value(defaultValue),
      m_maxValue(SHRT_MAX), m_minValue(SHRT_MIN), m_default(defaultValue),
      m_json(json_)
{
}

bool Setting::set(int value)
{
	if (value >= m_minValue && value <= m_maxValue)
	{
		m_value          = value;
		(*m_json)[m_key] = value;
		return true;
	}
	return false;
}

void Setting::reset() { set(m_default); }

void Setting::setMax(int value) { m_maxValue = value; }

void Setting::setMin(int value) { m_minValue = value; }

std::string Setting::getKey() const { return m_key; }

int Setting::value() const { return m_value; }

int Setting::getDefault() const { return m_default; }

Settings::Settings() : m_data(json::object()) {}

void Settings::registerAPI(mods::ModManager* manager)
{
	manager->registerFunction(
	    "core.setting.register",
	    [](std::string displayName, std::string key, int defaultValue) {
		    Settings::get()->add(displayName, key, defaultValue);
	    });

	manager->registerFunction("core.setting.get", [](std::string key) {
		return Settings::get()->getSetting(key)->value();
	});

	manager->registerFunction("core.setting.set",
	                          [](std::string key, int value) {
		                          Settings::get()->getSetting(key)->set(value);
	                          });
}

Setting* Settings::add(const std::string& name, const std::string& key,
                       int defaultValue)
{
	m_settings[key] = Setting(name, key, defaultValue, &m_data);
	return &m_settings[key];
}

Setting* Settings::getSetting(const std::string& key)
{
	if (m_data.find(key) != m_data.end())
	{
		if (m_settings.find(key) == m_settings.end())
		{
			add(key, key, m_data[key].get<int>());
		}
		return &m_settings[key];
	}
	else
	{
		m_data[key] = 0;
		return add(key, key, 0);
	}
}

void Settings::load(const std::string& saveFile)
{
	std::ifstream file;
	file.open(saveFile);
	if (file)
	{
		file >> m_data;
	}
	file.close();
}

void Settings::save(const std::string& saveFile)
{
	std::ofstream file;
	file.open(saveFile);
	file << std::setw(4) << m_data << std::endl;
	file.close();
}
