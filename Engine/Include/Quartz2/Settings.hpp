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

#pragma once

#include <Quartz2/Singleton.hpp>

#include <list>
#include <string>

namespace q2
{

	class Setting
	{
		/// @brief Human readable name for setting
		std::string m_name;
		/// @brief Unique name for key ex: core:volume
		std::string m_key;
		/// @brief Value of setting
		int m_value;
		/// @brief Maximim value that the setting can be set to
		int m_maxValue;
		/// @brief Minimum value that the setting cab be set to
		int m_minValue;

	public:
		Setting(std::string name, std::string key, int defaultValue);
		/**
		 * @brief Sets the value of an already existing setting
		 *
		 * @param value The value to be set
		 * @return true if the setting was set
		 * @return false if the value was not within the settings max/min
		 */
		bool set(int value);
		/**
		 * @brief Set the maximum value for the setting
		 *
		 * @param value The maximum value the setting can be
		 */
		void setMax(int value);
		/**
		 * @brief Set the minimum value for the setting
		 *
		 * @param value The minimum value the setting can be
		 */
		void setMin(int value);
		/**
		 * @brief Gets the value of a setting
		 *
		 * @return std::string the unique key for the setting
		 */
		std::string getKey();
		/**
		 * @brief Gets the value of a setting
		 *
		 * @return std::size_t The value of the setting
		 */
		int value();
	};

	class Settings : public Singleton<Settings>
	{
		std::list<Setting> m_setting;

	public:
		/**
		 * @brief Adds a new setting
		 *
		 * @param name Human readable name for setting
		 * @param key Unique Name for key ex: core:volume
		 * @param defaultValue The value the setting will be initially set to
		 * @return std::size_t Reuturns the numerical key the setting is stored
		 * at
		 */
		Setting* add(const std::string& name, const std::string& key, int defaultValue);

        /**
         * @brief Get the Setting object
         * 
         * @param key Unique Name for key ex: core:volume
         * @return Setting* A pointer to the setting object
         */
		Setting* getSetting(const std::string& key);
	};
}; // namespace q2