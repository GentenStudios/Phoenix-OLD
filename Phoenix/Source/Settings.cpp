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

#include <Phoenix/Settings.hpp>
#include <climits>
#include <fstream>
#include <iostream>

using namespace phx;

Setting::Setting() {}

Setting::Setting(std::string name, std::string key, int defaultValue)
    : m_name(std::move(name)), m_key(std::move(key)), m_value(defaultValue),
      m_maxValue(SHRT_MAX), m_minValue(SHRT_MIN), m_default(defaultValue)
{
}

bool Setting::set(int value)
{
	if (value >= m_minValue && value <= m_maxValue)
	{
		m_value = value;
		return true;
	}
	return false;
}

void Setting::reset() { m_value = m_default; }

void Setting::setMax(int value) { m_maxValue = value; }

void Setting::setMin(int value) { m_minValue = value; }

std::string Setting::getKey() const { return m_key; }

int Setting::value() const { return m_value; }

int Setting::getDefault() const { return m_default; }

Setting* Settings::add(const std::string& name, const std::string& key,
                       int defaultValue)
{
	m_settings[key] = Setting(name, key, defaultValue);
	return &m_settings[key];
}

Setting* Settings::getSetting(const std::string& key)
{
	if (m_settings.find(key) != m_settings.end())
		return &m_settings[key];
	else
		return nullptr;
}

void Settings::load()
{
	std::ifstream file;
	std::string   buffer;
	file.open("settings");
	if (file)
	{
		while (file >> buffer)
		{
			size_t      pointA  = buffer.find(',');
			std::string key     = buffer.substr(0, pointA);
			Setting*    setting = getSetting(key);
			if (setting == nullptr)
			{
				m_unused += buffer + "\n";
			}
			else
			{
				size_t      pointB = buffer.find(';');
				std::string val =
				    buffer.substr(pointA + 1, pointB - pointA - 1);
				setting->set(std::stoi(val));
			}
		}
	}
	file.close();
}

void Settings::save()
{
	std::ofstream file;
	file.open("settings");
	file << m_unused;
	for (const auto& setting : m_settings)
	{
		if (setting.second.value() != setting.second.getDefault())
		{
			file << setting.first << "," << setting.second.value() << ";\n";
		}
	}
	file.close();
}