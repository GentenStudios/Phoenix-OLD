//
// Created by Nicolas Ricard on 19/04/2020.
//

#include "Common/Serialization/Serializer.hpp"
#include "Common/Serialization/Endian.hpp"

phx::Serializer& phx::Serializer::operator&(phx::ISerializable& value_)
{
	return value_ & *this;
}
phx::Serializer& phx::Serializer::operator&(bool& value_)
{
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(long& value_)
{
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer& phx::Serializer::operator&(std::uint8_t& value_)
{
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
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
	if (mode::read == m_mode)
	{
		read(value_);
	}
	else
	{
		write(value_);
	}
	return *this;
}
phx::Serializer::Serializer(phx::Serializer::mode mode_) : m_mode(mode_) {}
