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
#include <Common/Serialization/SharedTypes.hpp>

#include <cstddef>
#include <vector>
#include <string>

namespace phx
{
	class Serializer;
	class ISerializable
	{
	public:
		virtual Serializer& operator&(Serializer& serializer) = 0;
	};

	class Serializer
	{
	public:
		enum class Mode
		{
			READ,
			WRITE
		};

	public:
		explicit Serializer(Mode mode) : m_mode(mode) {}

		data::Data& getBuffer() { return m_buffer; }
		void  setBuffer(std::byte* data, std::size_t dataLength);
		void  setBuffer(const data::Data& data) { m_buffer = data; }

		Serializer& operator&(bool& value);
		Serializer& operator&(char& value);
		Serializer& operator&(unsigned char& value);
		Serializer& operator&(float& value);
		Serializer& operator&(double& value);

		Serializer& operator&(signed short& value);
		Serializer& operator&(signed int& value);
		Serializer& operator&(signed long long& value);
		Serializer& operator&(unsigned short& value);
		Serializer& operator&(unsigned int& value);
		Serializer& operator&(unsigned long long& value);

		template <typename T>
		Serializer& operator&(std::basic_string<T>& value);

		Serializer& operator&(ISerializable& value);

		static data::Data endp(Serializer& serializer);
		
	private:
		template <typename T>
		void push(const T& data);

		template <typename T>
		void push(const std::basic_string<T>& data);

		void push(ISerializable& data);

		template <typename T>
		void pop(T& data);

		template <typename T>
		void pop(std::basic_string<T>& data);		
	
	private:		
		Mode m_mode;
		data::Data m_buffer;
	};
} // namespace phx::data

#include <Common/Serialization/Serializer.inl>
