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

namespace phx
{
	inline void Serializer::setBuffer(std::byte* data, std::size_t dataLength)
	{
		m_buffer.clear();
		m_buffer.insert(m_buffer.begin(), data, data + dataLength);
	}

	inline Serializer& Serializer::operator<<(bool val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(char val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(unsigned char val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(float val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(double val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(std::int16_t val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(std::int32_t val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(std::int64_t val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(std::uint16_t val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(std::uint32_t val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(std::uint64_t val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(const ISerializable& val)
	{
		val >> *this;
		return *this;
	}

#ifdef PHX_INT32_EQUAL_LONG
	inline Serializer& Serializer::operator<<(long val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator<<(unsigned long val)
	{
		push(val);
		return *this;
	}
#endif

	template <typename T>
	Serializer& Serializer::operator<<(const std::basic_string<T>& val)
	{
		push(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(bool& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(char& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(unsigned char& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(float& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(double& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(std::int16_t& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(std::int32_t& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(std::int64_t& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(std::uint16_t& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(std::uint32_t& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(std::uint64_t& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(ISerializable& val)
	{
		val << *this;
		return *this;
	}

#ifdef PHX_INT32_EQUAL_LONG
	inline Serializer& Serializer::operator>>(long& val)
	{
		pop(val);
		return *this;
	}

	inline Serializer& Serializer::operator>>(unsigned long& val)
	{
		pop(val);
		return *this;
	}
#endif
	
	template <typename T>
	Serializer& Serializer::operator>>(std::basic_string<T>& val)
	{
		pop(val);
		return *this;
	}

	template <typename T>
	void Serializer::push(const T& data)
	{
		union
		{
			std::byte bytes[sizeof(T)];
			T         value;
		} value;

		value.value = data::endian::swapForNetwork(data);

		for (std::size_t i = 0; i < sizeof(T); ++i)
		{
			m_buffer.push_back(value.bytes[i]);
		}
	}

	template <typename T>
	void Serializer::push(const std::basic_string<T>& data)
	{
		// if T is the same as the number of bits in a char, it's a normal
		// std::string. This means that there is only 1 byte per character and
		// so you don't need to factor in any endianness changes.
		if constexpr (sizeof(T) == CHAR_BIT)
		{
			// this is faster than iterating through every character and
			// swapping endianness and essentially doing an unnecessary
			// endianness swap.

			// push size of string onto data at the end.
			// specify unsigned int otherwise it will waste space allocating a
			// 64 bit variable.
			push(static_cast<unsigned int>(data.length()));

			// previous end of the array, so we can append onto that - rather
			// than the new end.
			const std::size_t prevEnd = m_buffer.size();

			// +1 for null terminator.
			m_buffer.resize(m_buffer.size() + data.length() + 1);

			// convert string to array of std::byte and append to data array.
			std::transform(data.begin(), data.end(), m_buffer.begin() + prevEnd,
			               [](char c) { return std::byte(c); });
		}
		else
		{
			// push a new value for each character in the string.
			// the reason this exists is because there are different strings in
			// the standard library, them being 16bit and 32bit character
			// strings.
			push(static_cast<unsigned int>(data.length()));
			for (auto c : data)
			{
				push(c);
			}
		}
	}

	template <typename T>
	void Serializer::pop(T& data)
	{
		union
		{
			std::byte bytes[sizeof(T)];
			T         value;
		} value;

		std::memcpy(value.bytes, m_buffer.data(), sizeof(T));

		// basically pop front for the amount of bytes of data we're taking.
		m_buffer.erase(m_buffer.begin(), m_buffer.begin() + sizeof(T));

		data = data::endian::swapForHost(value.value);
	}

	template <typename T>
	void Serializer::pop(std::basic_string<T>& data)
	{
		if constexpr (sizeof(T) == CHAR_BIT)
		{
			unsigned int size;
			pop(size);

			data.resize(size);

			std::transform(m_buffer.begin(), m_buffer.begin() + size,
			               data.begin(),
			               [](std::byte byte) { return char(byte); });

			// basically pop front for the amount of bytes of data we're taking.
			m_buffer.erase(m_buffer.begin(), m_buffer.begin() + size);
		}
		else
		{
			union
			{
				std::byte bytes[sizeof(T)];
				T         c;
			} values;

			unsigned int size;
			pop(size);

			data.reserve(size);

			for (unsigned int i = 0; i < size; ++i)
			{
				pop(values.c);
				data.push_back(values.c);
			}
		}
	}
} // namespace phx
