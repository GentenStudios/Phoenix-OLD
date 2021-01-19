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

#include <Common/CMS/ModManager.hpp>
#include <Common/Singleton.hpp>

#include <nlohmann/json.hpp>

#include <string>

namespace phx
{
	namespace internal
	{
		// These templated structs are a form of "SFINAE". They allow a
		// method to check whether the template parameter passed is a
		// specialization of an existing class.
		// For example:
		//		IsSpecialisation<bool, std::vector>::value would be False.
		//		IsSpecialisation<std::vector<int>, std::vector>::value would
		// be True.
		//
		// If needed elsewhere in the future, this can be moved to a utility
		// file and namespaced into something that's more generic.

		template <typename T, template <typename...> class Ref>
		struct IsSpecialisation : std::false_type
		{
		};

		template <template <typename...> class Ref, typename... Args>
		struct IsSpecialisation<Ref<Args...>, Ref> : std::true_type
		{
		};
	} // namespace internal

	/**
	 * @brief Represents a setting/configuration read from the config file.
	 * @tparam T The underlying type of the setting.
	 *
	 * The only supported types as of right now are arithmetic types,
	 * std::strings and booleans. These values are also supported as part of
	 * an std::vector (you can ask for std::vector<std::string>, etc...)
	 */
	template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T> || std::is_same_v<std::string, T> || internal::IsSpecialisation<T, std::vector>::value, int> = 0>
	class Setting
	{
	public:
		using Type = T;

	public:
		explicit Setting(nlohmann::json* json) : m_setting(json) {}

		operator T() { return m_setting->get<T>(); }
		operator T() const { return m_setting->get<T>(); }

		T* getPointer() // NOLINT: ALl control paths are covered.
		{
			if constexpr (std::is_same_v<std::string, T>)
			{
				return m_setting->get_ptr<nlohmann::json::string_t*>();
			}
			else if constexpr (std::is_same_v<bool, T>)
			{
				m_setting->get_ptr<nlohmann::json::boolean_t*>();
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				return m_setting->get_ptr<nlohmann::json::number_float_t*>();
			}
			else if constexpr (std::is_unsigned_v<T>)
			{
				return m_setting->get_ptr<nlohmann::json::number_unsigned_t*>();
			}
			else
			{
				return m_setting->get_ptr<nlohmann::json::number_integer_t*>();
			}
		}

		const T* getPointer() const // NOLINT: ALl control paths are covered.
		{
			if constexpr (std::is_same_v<std::string, T>)
			{
				return m_setting->get_ptr<const nlohmann::json::string_t*>();
			}
			else if constexpr (std::is_same_v<bool, T>)
			{
				m_setting->get_ptr<const nlohmann::json::boolean_t*>();
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				return m_setting
				    ->get_ptr<const nlohmann::json::number_float_t*>();
			}
			else if constexpr (std::is_unsigned_v<T>)
			{
				return m_setting
				    ->get_ptr<const nlohmann::json::number_unsigned_t*>();
			}
			else
			{
				return m_setting
				    ->get_ptr<const nlohmann::json::number_integer_t*>();
			}
		}

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

	// can't use singleton cos the get function already exists. an instance
	// method will be used.
	class Settings
	{
	public:
		struct KeyValPair
		{
			std::string key;
			nlohmann::json* val;
		};
		
	public:
		Settings()  = default;
		~Settings() = default;

		static Settings* instance();

		void registerAPI(cms::ModManager* manager);
		
		// void registerAPI(cms::ModManager* manager);
		bool parse(const std::string& configFile);
		void save();
		void saveTo(const std::string& newConfig);
		bool exists(const std::string& key) const;

		/**
		 * @brief Gets the setting requested, creates one if necessary.
		 * @tparam Type The type being requested from the Settings object.
		 * @param key The key/name of the value being requested.
		 * @param skipVerif The option to skip validation if you have just
		 * successfully validated it.
		 * @return The setting object associated with the key provided.
		 *
		 * This method is expensive due to the checks involved. It checks
		 * for existence of the value before then validating the value
		 * itself.
		 *
		 * If the value is not valid, a new value will be made in another
		 * "settings object" (an nlohmann::json object), this prevents
		 * invalid values being overwritten in file and allows the program
		 * to continue in the case of an invalid input. The new value
		 * created will be default-initialized and set to the type that is
		 * being requested.
		 *
		 * If the value does not exist, it will simply be created and stored
		 * on save.
		 *
		 * If the parameter "skipVerif" is set to true, all the internal
		 * checks are skipped. This is useful if you validate a setting
		 * first and then get it - if the validation returns false however,
		 * skipVerif must not be enabled otherwise there will be issues
		 * during usage.
		 *
		 */
		template <typename Type>
		Setting<Type> get(const std::string& key, bool skipVerif = false)
		{
			if (skipVerif)
			{
				return Setting<Type>(&m_settings[key]);
			}

			if (exists(key))
			{
				if (valid<Type>(key))
				{
					return Setting<Type>(&m_settings[key]);
				}
				else
				{
					LOG_WARNING("CONFIG")
					    << "An incorrect value was provided "
					       "for the configuration parameter: "
					    << key << " using a default value instead.";

					if (m_invalidOverwriter.find(key) ==
					    m_invalidOverwriter.end())
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

		// same as above but with a default value provided.
		template <typename Type>
		Setting<Type> getOr(const std::string& key, Type&& defaultValue)
		{
			if (exists(key))
			{
				if (valid<Type>(key))
				{
					return Setting<Type>(&m_settings[key]);
				}
				else
				{
					LOG_WARNING("CONFIG")
					    << "An incorrect value was provided "
					       "for the configuration parameter: "
					    << key << " using a default value instead.";

					if (m_invalidOverwriter.find(key) ==
					    m_invalidOverwriter.end())
					{
						m_invalidOverwriter[key] =
						    std::forward<Type>(defaultValue);
					}

					return Setting<Type>(&m_invalidOverwriter[key]);
				}
			}
			else
			{
				m_settings[key] = std::forward<Type>(defaultValue);
				return Setting<Type>(&m_settings[key]);
			}
		}

		// clang-format off
		// have to turn clang-format off here otherwise it formats funkily.
			
		/**
		 * @brief Validates the data type found at the value associated with requested key.
		 * @tparam Type The underlying datatype, vector-based data type being validated.
		 * @param key The actual key/name of the setting itself.
		 * @return Whether the value paired with the key is valid or not.
		 *
		 * @notes Returns false if not exists too.
		 *        Only arithmetic types, strings and booleans are supported as of right now.
		 */
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

		/**
		 * @brief Validates the data type found at the value associated with requested key.
		 * @tparam Type The underlying datatype, POD/string data type being validated.
		 * @param key The actual key/name of the setting itself.
		 * @return Whether the value paired with the key is valid or not.
		 *
		 * @notes Returns false if not exists too.
		 *        Only arithmetic types, strings and booleans are supported as of right now.
		 */
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

		std::vector<KeyValPair> getImplFinalSettings();

	private:
		/**
		 * @brief The filepath to the config file.
		 */
		std::string m_configFilePath = "PhoenixConfig.json";

		/**
		 * @brief The main settings object.
		 *
		 * This settings object is what a .json config file is read into.
		 * Anything in this object is stored in the file.
		 */
		nlohmann::json m_settings;
		
		/**
		 * @brief The secondary settings object for primarily in-ram
		 * settings.
		 *
		 * This settings object is important for the potential cases of
		 * invalid input by the user. It provides a way to create a
		 * temporary object in RAM for the application to rely on. No values
		 * in here are read from file, and vice versa - no values from this
		 * object are written to the file.
		 */
		nlohmann::json m_invalidOverwriter;
	};
}; // namespace phx
