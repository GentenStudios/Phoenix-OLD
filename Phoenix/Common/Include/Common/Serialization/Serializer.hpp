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
#include <Common/Serialization/Endian.hpp>
#include <cstddef>
#include <functional>
#include <vector>

#if defined(__APPLE__)
#	define __INT32_EQUAL_LONG__ 1
#elif defined(_WIN32)
#	define __INT32_EQUAL_LONG__ 1
#else
#	define __INT32_EQUAL_LONG__ 0
#endif

namespace phx
{
	class Serializer;

	class ISerializable
	{
	public:
		virtual phx::Serializer& operator&(phx::Serializer& this_) = 0;
	};

	class Serializer
	{
	public:
		enum class Mode
		{
			READ,
			WRITE
		};
		void setBuffer(std::byte* data_, size_t dataLenght_)
		{
			buffer.clear();
			buffer.insert(buffer.begin(), data_, data_ + dataLenght_);
		}
		explicit Serializer(Mode mode_);
		phx::Serializer& operator&(ISerializable& value_);
		phx::Serializer& operator&(bool& value_);
		phx::Serializer& operator&(char& value_);
#if __INT32_EQUAL_LONG__
		phx::Serializer& operator&(long& value_);
		phx::Serializer& operator&(unsigned long& value_);
#endif
		phx::Serializer&              operator&(std::uint8_t& value_);
		phx::Serializer&              operator&(std::int8_t& value_);
		phx::Serializer&              operator&(std::uint16_t& value_);
		phx::Serializer&              operator&(std::int16_t& value_);
		phx::Serializer&              operator&(std::uint32_t& value_);
		phx::Serializer&              operator&(std::int32_t& value_);
		phx::Serializer&              operator&(std::uint64_t& value_);
		phx::Serializer&              operator&(std::int64_t& value_);
		phx::Serializer&              operator&(std::string& value_);
		phx::Serializer&              operator&(std::wstring& value_);
		static std::vector<std::byte> endp(phx::Serializer& this_);
		std::vector<std::byte>        operator&(
            std::function<std::vector<std::byte>(phx::Serializer&)> function);

	private:
		template <typename T>
		void read(T& value_)
		{
			phx::word<T> word(buffer.data(), 1);
			value_ = word.from_network();
			buffer.erase(buffer.begin(), buffer.begin() + word.size);
		}
		template <typename char_type>
		void read(std::basic_string<char_type>& value_)
		{
			std::size_t size;
			read(size);
			phx::word<char_type*> word((std::byte*) buffer.data(), size);
			word.from_network();
			value_ = std::basic_string<char_type>(word.value.data, size);
			buffer.erase(buffer.begin(), buffer.begin() + size + 1);
		}
		template <typename T>
		void write(T& value_)
		{
			phx::word<T> word(value_);
			word.to_network();
			buffer.insert(buffer.end(), word.value.bytes,
			              word.value.bytes + word.size);
		}
		template <typename char_type>
		void write(std::basic_string<char_type>& value_)
		{
			std::size_t size = value_.size();
			write(size);
			phx::word<char_type*> word((std::byte*) value_.data(), size);
			word.to_network();
			buffer.insert(buffer.end(), word.value.bytes,
			              word.value.bytes + word.size);
			char_type char0 {};
			/// @todo: check if next line is needed
			write(char0);
		}

	private:
		Mode                   m_mode;
		std::vector<std::byte> buffer;
	};
} // namespace phx
