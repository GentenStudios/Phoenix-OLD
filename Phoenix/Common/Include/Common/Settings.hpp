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

#include <Common/Singleton.hpp>
#include <Common/CMS/ModManager.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <unordered_map>
#include <algorithm>
#include <type_traits>

using json = nlohmann::json;

namespace phx
{
	/**
	 * @brief A settings object to store a single setting.
	 *
	 * @description The settings object stores an integer that can be mapped to
	 * other data. Maximum and minimum values can be set so a settings cant be
	 * adjusted outside its limits.
	 *
	 */
	class Setting
	{
		/// @brief Human readable name for setting.
		std::string m_name;

		/// @brief Unique name for key ex: core:volume.
		std::string m_key;

		/// @brief Value of setting.
		int m_value;

		/// @brief Default value for setting.
		int m_default;

		/// @brief Maximum value that the setting can be set to.
		int m_maxValue;

		/// @brief Minimum value that the setting can be set to.
		int m_minValue;

		/// @brief A pointer to the JSON object to be able to update it
		json* m_json;

	public:
		Setting() = default;

		/**
		 * @brief Construct a new Setting object.
		 *
		 * @param name The human readable name for the setting.
		 * @param key The unique name for the setting in the format core:volume.
		 * @param defaultValue The default value for the setting upon creation.
		 */
		Setting(std::string name, std::string key, int defaultValue, json* json_);
	
		/**
		 * @brief Sets the value of an already existing setting.
		 *
		 * @param value The value to be set.
		 * @return true if the setting was set.
		 * @return false if the value was not within the settings max/min.
		 */
		bool set(int value);

		/**
		 * @brief Resets the value of the setting back to the default.
		 *
		 */
		void reset();

		/**
		 * @brief Set the maximum value for the setting.
		 *
		 * @param value The maximum value the setting can be.
		 */
		void setMax(int value);
		int  getMax() const { return m_maxValue; }

		/**
		 * @brief Set the minimum value for the setting.
		 *
		 * @param value The minimum value the setting can be.
		 */
		void setMin(int value);
		int  getMin() const { return m_minValue; }

		/**
		 * @brief Gets the value of a setting.
		 *
		 * @return std::string the unique key for the setting.
		 */
		const std::string& getKey() const;

		const std::string& getName() const;

		/**
		 * @brief Gets the value of a setting.
		 *
		 * @return std::size_t The value of the setting.
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
	 * @brief A setting registry to store settings for universal access.
	 *
	 * @description This registry allows us to access any registered setting
	 * from anywhere in the program just using the settings unique key in the
	 * format core:volume.
	 *
	 */
	class Settings : public Singleton<Settings>
	{
	public:
		Settings();

		void registerAPI(cms::ModManager* manager);

		/**
		 * @brief Adds a new setting.
		 *
		 * @param name Human readable name for setting.
		 * @param key Unique Name for key ex: core:volume.
		 * @param defaultValue The value the setting will be initially set to.
		 * @return std::size_t Returns the numerical key the setting is stored
		 * at.
		 */
		Setting* add(const std::string& name, const std::string& key,
		             int defaultValue);

		/**
		 * @brief Get the Setting object.
		 *
		 * @param key Unique Name for key ex: core:volume.
		 * @return Setting* A pointer to the setting object.
		 */
		Setting* getSetting(const std::string& key);

		const std::unordered_map<std::string, Setting>& getSettings();

		/**
		 * @brief Loads settings from file.
		 *
		 * @note this must be run after all settings have been registered.
		 */
		void load(const std::string& saveFile);

		/**
		 * @brief Saves settings to file.
		 */
		void save(const std::string& saveFile);

	private:
		std::unordered_map<std::string, Setting> m_settings;
		json m_data;
	};
}; // namespace phx

namespace phx
{
	namespace settings
	{
		namespace internal
		{
			// These templated structs are a form of "SFINAE". They allow a
			// method to check whether the template parameter passed is a
			// specialization of an existing class.
			// For example:
			//		IsSpecialisation<bool, std::vector>::value would be False.
			//		IsSpecialisation<std::vector<int>, std::vector>::value would be True.
			//
			// If needed elsewhere in the future, this can be moved to a utility
			// file and namespaced into something that's more generic.
			
			template <typename T, template<typename...> class Ref>
			struct IsSpecialisation : std::false_type
			{
			};
		
			template <template<typename...> class Ref, typename... Args>
			struct IsSpecialisation<Ref<Args...>, Ref> : std::true_type
			{
			};
		}
		
		/**
		 * @brief Represents a setting/configuration read from the config file.
		 * @tparam T The underlying type of the setting.
		 *
		 * The only supported types as of right now are arithmetic types,
		 * std::strings and booleans. These values are also supported as part of
		 * an std::vector (you can ask for std::vector<std::string>, etc...)
		 */
		template <typename T>
		class Setting
		{
		public:
			using Type = T;
			
		public:
			explicit Setting(nlohmann::json* json) : m_setting(json) {}

			explicit operator T() { return m_setting->get<T>(); }
			explicit operator T() const { return m_setting->get<T>(); }
			explicit operator T*() { return m_setting->get_ptr<T*>(); }
			explicit operator const T*() const { return m_setting->get_ptr<const T*>(); }

			Setting& operator=(const Setting& rhs)
			{
				m_setting = rhs.m_setting;
				return *this;
			}

			Setting& operator=(const T& rhs)
			{
				*m_setting = rhs;
				return *this;
			}

			Setting& operator=(T&& rhs)
			{
				*m_setting = std::move(rhs);
				return *this;
			}

		private:
			nlohmann::json* m_setting;
		};

		class SettingsManager : Singleton<SettingsManager>
		{
		public:
			SettingsManager() = default;
			~SettingsManager() = default;

			void registerAPI(cms::ModManager* manager);
			void parse(const std::string& configFile);
			void save();

			bool exists(const std::string& key) const
			{
				return m_settings.find(key) != m_settings.end();
			}

			template <typename Type>
			Setting<Type> get(const std::string& key)
			{
				bool doesExist = exists(key);
				if (doesExist)
				{
					bool result = valid<Type>(key);
					if (result)
					{
						return Setting<Type>(&m_settings[key]);
					}
					else
					{
						LOG_WARNING("CONFIG")
						    << "An incorrect value was provided "
						       "for the configuration parameter: "
						    << key << " using a default value instead.";

						if (m_invalidOverwriter.find(key) == m_invalidOverwriter.end())
						{
							m_invalidOverwriter[key] = Type {};
						}
						
						return Setting<Type>(&m_invalidOverwriter[key]);
					}
				}
				else
				{
					m_settings[key] = Type {};
					return Setting<Type>(&m_settings[key]);
				}
			}
			
			// have to turn clang-format off here otherwise it's actually
			// painful...
			// clang-format off
			template <typename Type>
			std::enable_if_t<internal::IsSpecialisation<Type, std::vector>::value, bool> valid(const std::string& key) const
			{
				const auto it = m_settings.find(key);
				if (it != m_settings.end())
				{
					if (it->is_array())
					{
						// check what type we're working with.
						// we have to do std::all_of to make sure all the values are that type otherwise it throws an exception.
						// we currently only support arithmetic types, std::string and booleans - which really should be enough.
						if constexpr (std::is_arithmetic_v<typename Type::value_type>)
						{
							return std::all_of(it->begin(), it->end(),
							                   [](const nlohmann::json& val) {
								                   return val.is_number();
							                   });
						}
						else if constexpr (std::is_same_v<typename Type::value_type, std::string>)
						{
							return std::all_of(it->begin(), it->end(),
							                   [](const nlohmann::json& val) {
								                   return val.is_string();
							                   });	
						}
						else if constexpr (std::is_same_v<typename Type::value_type, bool>)
						{
							return std::all_of(it->begin(), it->end(),
							                   [](const nlohmann::json& val) {
								                   return val.is_boolean();
							                   });	
						}
					}
				}

				return false;
			}

			template <typename Type>
			std::enable_if_t<std::is_arithmetic_v<Type> || std::is_same_v<Type, std::string> || std::is_same_v<Type, bool>, bool> valid(const std::string& key) const
			{
				const auto it = m_settings.find(key);
				if (it != m_settings.end())
				{
					if      constexpr (std::is_arithmetic_v<Type>)        { return it->is_number(); }
					else if constexpr (std::is_same_v<Type, std::string>) { return it->is_string(); }
					else if constexpr (std::is_same_v<Type, bool>)        { return it->is_boolean(); }
				}

				return false;
			}
			// clang-format on
			
		private:
			// path to the config file itself.
			std::string m_configFilePath;
			
			// main settings storage object.
			nlohmann::json m_settings;

			// rather than overwriting invalid values, we just store and modify
			// them in memory. we try to use this is least as we can.
			nlohmann::json m_invalidOverwriter;
		};
	}
}
