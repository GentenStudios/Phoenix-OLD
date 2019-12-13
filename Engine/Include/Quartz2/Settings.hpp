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

#include <string>
#include <vector>

namespace q2 {

    struct Setting {
    /// @brief Human readable name for setting
    std::string name;
    /// @brief Unique name for key ex: core:volume
    std::string key;
    /// @brief Value of setting
    std::size_t value;
    };

    class Settings : public Singleton<Settings> {
    std::vector<Setting> m_setting;

    public:
    /**
     * @brief Adds a new setting
     *
     * @param name Human readable name for setting
     * @param key Unique Name for key ex: core:volume
     * @param defaultValue The value the setting will be initially set to
     * @return std::size_t Reuturns the numerical key the setting is stored at
     */
    std::size_t add(std::string name, std::string key, std::size_t defaultValue);
    /**
     * @brief Sets the value of an already existing setting
     *
     * @param value The value to be set
     * @param key The unique name for key ex: core:volume
     * @return true if the setting was set
     * @return false if the setting does not exist
     */
    bool set(std::size_t key, std::size_t value);
    /**
     * @brief Sets the value of an already existing setting
     *
     * @param value The value to be set
     * @param key The numerical key the setting is stored at
     * @return true if the setting was set
     * @return false if the setting does not exist
     */
    bool set(std::string key, std::size_t value);
    /**
     * @brief Gets the value of a setting
     *
     * @param key The unique name for key ex: core:volume
     * @return std::size_t The value of the setting
     */
    std::size_t value(std::string key);
    /**
     * @brief Gets the value of a setting
     *
     * @param key The numerical key the setting is stored at
     * @return std::size_t The value of the setting
     */
    std::size_t value(std::size_t key);

    Setting *getSetting(std::size_t key);
};
}; // namespace q2