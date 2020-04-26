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

#include <Common/Serialization/Endian.hpp>
#include <Common/Serialization/Serializer.hpp>

phx::Serializer& phx::Serializer::operator&(phx::ISerializable& value_)
{
	return value_ & *this;
}
phx::Serializer& phx::Serializer::operator&(bool& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(char& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
#if __INT32_EQUAL_LONG__
phx::Serializer& phx::Serializer::operator&(long& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(unsigned long& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
#endif
phx::Serializer& phx::Serializer::operator&(std::uint8_t& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::uint16_t& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::int16_t& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::uint32_t& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::int32_t& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::uint64_t& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::int64_t& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::string& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::wstring& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}

std::vector<std::byte> phx::Serializer::endp(phx::Serializer& this_)
{
	return this_.buffer;
}
std::vector<std::byte> phx::Serializer::operator&(
    std::function<std::vector<std::byte>(phx::Serializer&)> function_)
{
	return function_(*this);
}
phx::Serializer& phx::Serializer::operator&(int8_t& value_)
{
	if (Mode::READ == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer::Serializer(phx::Serializer::Mode mode_) : m_mode(mode_) {}
void phx::Serializer::setBuffer(std::byte* data_, size_t dataLength_)
{
	buffer.clear();
	buffer.insert(buffer.begin(), data_, data_ + dataLength_);
}
