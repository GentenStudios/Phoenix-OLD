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
 * @file Metadata.hpp
 * @brief Used to store unique data per individual instances
 *
 * @copyright Copyright (c) Genten Studios 2019 - 2020
 *
 */

#pragma once

#include <Common/Utility/Serializer.hpp>

#include <any>
#include <string>
#include <unordered_map>

namespace phx
{

	class Metadata : public ISerializable
	{
	public:
		/**
		 * @brief Sets or inserts metadata.
		 * @param key The key associated with the metadata.
		 * @return true If the data was set.
		 * @return false If the data already exists with a different data type
		 * or an incompatible data type was provided.
		 */
		bool set(const std::string& key, const std::any& existing);

		/**
		 * @brief Gets metadata by key.
		 * @param key The key associated with the metadata.
		 * @return A pointer to the data.
		 */
		const std::any* get(const std::string& key) const;

		/**
		 * @brief Erases metadata.
		 * @param key The key associated with the metadata.
		 */
		void erase(const std::string& key) { m_data.erase(key); };

		/**
		 * @return the size of the contained map object.
		 */
		size_t size() { return m_data.size(); };

		// serialize.
		Serializer& operator>>(Serializer& ser) const override;

		// deserialize.
		Serializer& operator<<(Serializer& ser) override;

	private:
		std::unordered_map<std::string, std::any> m_data;
	};
}