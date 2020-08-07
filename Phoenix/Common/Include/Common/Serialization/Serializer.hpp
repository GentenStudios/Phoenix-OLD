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
#include <cstring>

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

	/**
	 * @brief Interface class for helping with data structures.
	 *
	 * This function must be overridden, usage for the Serializer can be found
	 * below.
	 */
	class ISerializable
	{
	public:
		// serialize.
		virtual Serializer& operator>>(Serializer& serializer) const = 0;

		// unserialize.
		virtual Serializer& operator<<(Serializer& serializer) = 0;
	};

	/**
	 * @brief Serializes data with Endianness correction for network transfer.
	 *
	 * This class provides a way to safely accumulate data with automatic
	 * correction for system endianness for transfer over a network. You can use
	 * this class on both sides of the system.
	 *
	 * @paragraph Usage
	 * If preparing data to send, the mode used must be Mode::WRITE, since
	 * you're writing to the buffer. If you've just received data and funneled
	 * the Packet's data into the serializer, you should use the Mode::READ
	 * mode. This will prevent you from writing to the buffer (and vice versa
	 * for the write mode).
	 *
	 * To pack a value into the buffer, use the & operator and write mode. The &
	 * operator is small so you can have a chain of things like: ``serializer &
	 * var & var2 & var3;``. The buffer can then be retrieved using ``getBuffer()``
	 *
	 * To retrieve a value from the buffer, again, use the & operator and read
	 * mode. You should read **in the same direction of variables than when
	 * you packed the buffer**. For example, pack the buffer like this:
	 * @code
	 * Serializer ser(Serializer::Mode::WRITE);
	 * ser & var1 & var2 & var3 & var4;
	 * @endcode
	 * And unpack the buffer like this:
	 * @code
	 * Serializer ser(Serializer::Mode::READ);
	 * ser & var1 & var2 & var3 & var4;
	 * @endcode
	 *
	 * Usage with a packet:
	 * @code
	 * // client:
	 * int status = 5;
	 * bool moving = true;
	 * float wowee = 0.01f;
	 * std::size_t sequence = 100355;
	 *
	 * Serializer ser(Serializer::Mode::WRITE)
	 * ser & status & moving & wowee & sequence;
	 *
	 * send_packet(ser.getBuffer());
	 *
	 * // server:
	 * int status;
	 * bool moving;
	 * float wowee;
	 * std::size_t sequence;
	 *
	 * Packet packet = receive_packet();
	 *
	 * Serializer ser(Serializer::Mode::READ)
	 * ser.setBuffer(packet.getData());
	 * ser & status & moving & wowee & sequence;
	 *
	 * // status, moving, wowee and sequence will be equal to their client
	 * // counterparts.
	 * @endcode
	 */
	class Serializer
	{
	public:
		enum class Mode
		{
			READ,
			WRITE
		};

	public:
		Serializer() = default;

		data::Data& getBuffer() { return m_buffer; }
		void  setBuffer(std::byte* data, std::size_t dataLength);
		void  setBuffer(const data::Data& data) { m_buffer = data; }
		void  setBuffer(data::Data&& data) { m_buffer = std::move(data); }

		Serializer& operator<<(bool val);
		Serializer& operator<<(char val);
		Serializer& operator<<(unsigned char val);
		Serializer& operator<<(float val);
		Serializer& operator<<(double val);
		Serializer& operator<<(std::int16_t val);
		Serializer& operator<<(std::int32_t val);
		Serializer& operator<<(std::int64_t val);
		Serializer& operator<<(std::uint16_t val);
		Serializer& operator<<(std::uint32_t val);
		Serializer& operator<<(std::uint64_t val);
		Serializer& operator<<(const ISerializable& val);

		template <typename T>
		Serializer& operator<<(const std::basic_string<T>& val);

		Serializer& operator>>(bool& val);
		Serializer& operator>>(char& val);
		Serializer& operator>>(unsigned char& val);
		Serializer& operator>>(float& val);
		Serializer& operator>>(double& val);
		Serializer& operator>>(std::int16_t& val);
		Serializer& operator>>(std::int32_t& val);
		Serializer& operator>>(std::int64_t& val);
		Serializer& operator>>(std::uint16_t& val);
		Serializer& operator>>(std::uint32_t& val);
		Serializer& operator>>(std::uint64_t& val);
		Serializer& operator>>(ISerializable& val);

		template <typename T>
		Serializer& operator>>(std::basic_string<T>& val);
		
	private:
		template <typename T>
		void push(const T& data);

		template <typename T>
		void push(const std::basic_string<T>& data);

		template <typename T>
		void pop(T& data);

		template <typename T>
		void pop(std::basic_string<T>& data);

	public:
		Mode m_mode;

	private:
		data::Data m_buffer;
	};
} // namespace phx::data

#include <Common/Serialization/Serializer.inl>
