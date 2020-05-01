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

#include <Common/Logger.hpp>
#include <Common/Network/Packet.hpp>

using namespace phx::net;

Packet::Packet(const Data& data, PacketFlags flags)
{
	// unreliable is fake just cos so removing it.
	create(data, flags & ~PacketFlags::UNRELIABLE);
}

Packet::Packet(std::size_t size, PacketFlags flags)
    : Packet(*enet_packet_create(
          nullptr, size,
          static_cast<enet_uint32>(flags & ~PacketFlags::UNRELIABLE)))
{
}

Packet::Packet(ENetPacket& packet, bool sent) : m_packet(&packet), m_sent(sent)
{
}

Packet::~Packet()
{
	if (!m_sent)
	{
		enet_packet_destroy(m_packet);
	}
}

void Packet::setData(const Data& data)
{
	if (m_sent)
	{
		// packet has already been sent.
		LOG_DEBUG("NETCODE") << "Packet has already been sent.";
		return;
	}

	if (data.size() != m_packet->dataLength)
	{
		enet_packet_resize(m_packet, data.size());
	}

	for (std::size_t i = 0; i < data.size(); ++i)
	{
		enet_uint8* ptr = m_packet->data + i;
		*ptr            = static_cast<enet_uint8>(data[i]);
	}
}

Packet& Packet::operator=(const Data& data)
{
	if (m_sent)
	{
		// packet has already been sent.
		LOG_DEBUG("NETCODE") << "Packet has already been sent.";
		return *this;
	}

	setData(data);

	return *this;
}

Packet::Data Packet::getData() const
{
	return {
	    reinterpret_cast<std::byte*>(m_packet->data),
	    reinterpret_cast<std::byte*>(m_packet->data + m_packet->dataLength)};
}

void Packet::resize(std::size_t size)
{
	if (m_sent)
	{
		// packet has already been sent.
		LOG_DEBUG("NETCODE") << "Packet has already been sent.";
		return;
	}

	enet_packet_resize(m_packet, size);
}

std::size_t Packet::getSize() const { return m_packet->dataLength; }

void Packet::prepareForSend() { m_sent = true; }

void Packet::create(const Data& data, PacketFlags flags)
{
	m_packet = enet_packet_create(data.data(), data.size(),
	                              static_cast<enet_uint32>(flags));
}
