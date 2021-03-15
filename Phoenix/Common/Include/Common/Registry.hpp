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

#pragma once

#include <unordered_map>

namespace phx
{
	template <typename Key, typename Value>
	class Registry
	{
	public:
		using Iterator = typename std::unordered_map<Key, Value>::iterator;
		using ConstIterator =
		    typename std::unordered_map<Key, Value>::const_iterator;

	public:
		Registry()  = default;
		~Registry() = default;

		void add(const Key& key, const Value& value)
		{
			m_registry.insert_or_assign(key, value);
		}

		void add(Key&& key, Value&& value) { m_registry.emplace(key, value); }

		Value* get(const Key& key) const
		{
			auto it = m_registry.find(key);
			if (it == m_registry.end())
			{
				return m_unknownValueReturnVal;
			}

			return &it->second;
		}

		Value* get(const Key& key)
		{
			auto it = m_registry.find(key);
			if (it == m_registry.end())
			{
				return m_unknownValueReturnVal;
			}

			return &it->second;
		}

		// use this to return a specific value if not found in the registry.
		// will otherwise return nullptr;
		void setUnknownReturnVal(Value* value)
		{
			m_unknownValueReturnVal = value;
		}
		
		Iterator      begin() { return m_registry.begin(); }
		Iterator      end() { return m_registry.end(); }
		ConstIterator begin() const { return m_registry.begin(); }
		ConstIterator end() const { return m_registry.end(); }
		std::size_t   size() const { return m_registry.size(); }
		bool          empty() const { return m_registry.empty(); }

	private:
		std::unordered_map<Key, Value> m_registry;

		Value* m_unknownValueReturnVal = nullptr;
	};
} // namespace phx
