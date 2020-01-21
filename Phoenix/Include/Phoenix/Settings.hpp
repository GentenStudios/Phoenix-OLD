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

/**
 * @file Settings.hpp
 * @brief Implements a settings management system
 *
 * @copyright Copyright (c) 2019-20 Genten Studios
 *
 */

#pragma once

#include <Phoenix/Singleton.hpp>

#include <string>
#include <unordered_map>

namespace phx
{
	/**
	 * @brief A settings object to store a single setting
	 *
	 * @description The settings object stores an integer that can be mapped to
	 * other data. Maximum and minimum values can be set so a settings cant be
	 * adjusted outside its limits.
	 *
	 */
	class Setting
	{
		/// @brief Human readable name for setting
		std::string m_name;
		/// @brief Unique name for key ex: core:volume
		std::string m_key;
		/// @brief Value of setting
		int m_value;
		/// @brief Default value for setting
		int m_default;
		/// @brief Maximim value that the setting can be set to
		int m_maxValue;
		/// @brief Minimum value that the setting cab be set to
		int m_minValue;

	public:
		Setting();
		/**
		 * @brief Construct a new Setting object
		 *
		 * @param name The human readable name for the setting
		 * @param key The unique name for the setting in the format core:volume
		 * @param defaultValue The default value for the setting upon creation
		 */
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
		 * @brief Resets the value of the setting back to the default
		 *
		 */
		void reset();
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
		std::string getKey() const;
		/**
		 * @brief Gets the value of a setting
		 *
		 * @return std::size_t The value of the setting
		 */
		int value() const;
		/**
		 * @brief Gets the default of a setting
		 *
		 * @return std::size_t The value of the setting
		 */
		int getDefault() const;
	};

	/**
	 * @brief A setting registry to store settings for universal access
	 *
	 * @description This registry allows us to access any registered setting
	 * from anywhere in the program just using the settings unique key in the
	 * format core:volume
	 *
	 */
	class Settings : public Singleton<Settings>
	{
		std::unordered_map<std::string, Setting> m_settings;
		std::string                              m_unused;

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
		Setting* add(const std::string& name, const std::string& key,
		             int defaultValue);

		/**
		 * @brief Get the Setting object
		 *
		 * @param key Unique Name for key ex: core:volume
		 * @return Setting* A pointer to the setting object
		 */
		Setting* getSetting(const std::string& key);

		/**
		 * @brief Loads settings from file
		 *
		 * @note this must be run after all settings have been registered
		 */
		void load();

		/**
		 * @brief Saves settings to file
		 */
		void save();
	};
}; // namespace phx