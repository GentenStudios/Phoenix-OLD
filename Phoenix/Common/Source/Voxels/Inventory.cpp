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
#include <Common/Voxels/Inventory.hpp>

using namespace phx::voxels;

Inventory::Inventory(std::size_t size, ItemReferrer* referrer)
    : m_size(size), m_referrer(referrer)
{
	for (std::size_t i = 0; i < size; i++)
	{
		m_slots.emplace_back(nullptr);
	}
}

Item Inventory::getItem(std::size_t slot)
{
	if (m_slots.size() >= slot)
	{
		if (m_metadata.find(slot) != m_metadata.end())
		{
			return {m_slots[slot], m_metadata[slot]};
		}
		return {m_slots[slot], nullptr};
	}
	return {m_referrer->items.get(ItemType::OUT_OF_BOUNDS_ITEM), nullptr};
}

bool Inventory::addItem(std::size_t slot, const Item& item)
{
	if (getItem(slot).type == nullptr && m_size >= slot)
	{
		m_slots[slot] = item.type;
		if (item.metadata != nullptr)
		{
			m_metadata[slot] = item.metadata;
		}
		return true;
	}
	LOG_DEBUG("Inv") << getItem(slot).type << m_size << slot;
	return false;
}

int Inventory::addItem(const Item& item)
{
	for (std::size_t i = 0; i < m_size; i++)
	{
		if (m_slots.at(i) == nullptr)
		{
			m_slots[i] = item.type;
			if (item.metadata != nullptr)
			{
				m_metadata[i] = item.metadata;
			}
			return i;
		}
	}
	return -1;
}

Item Inventory::removeItem(std::size_t slot)
{
	if (m_slots.size() >= slot)
	{
		Item item;
		item.type     = m_slots[slot];
		m_slots[slot] = nullptr;
		if (m_metadata.find(slot) != m_metadata.end())
		{
			item.metadata = m_metadata[slot];
			m_metadata.erase(slot);
		}
		return item;
	}
	return {m_referrer->items.get(ItemType::OUT_OF_BOUNDS_ITEM), nullptr};
}

/**
 * @TODO Should we return a tuple with an error type here? There are two things
 * that could go wrong either the block is OOB or the metadata type is invalid.
 * Or should we just assert on the second error?
 */
bool Inventory::setMetadataAt(std::size_t slot, const std::string& key,
                              std::any* newData)
{
	if (m_size <= slot)
	{
		return m_metadata[slot]->set(key, newData);
	}
	LOG_DEBUG("Inventory.cpp")
	    << "Attempted to set metadata for out of bounds inventory slot";
	return false;
}

phx::Serializer& Inventory::operator>>(phx::Serializer& ser) const
{
	ser << m_size;
	for (std::size_t i = 0; i < m_size; i++)
	// for (const BlockType* block : m_blocks)
	{
		ser << m_slots[i]->uniqueIdentifier;
		if (m_metadata.find(i) != m_metadata.end())
		{
			ser << '+' << *m_metadata.at(i);
		}
		else
		{
			ser << ';';
		}
	}
	return ser;
}

phx::Serializer& Inventory::operator<<(phx::Serializer& ser)
{
	ser >> m_size;
	m_slots.reserve(m_size);
	for (std::size_t i = 0; i < m_size; i++)
	{
		std::size_t id = 0;
		ser >> id;
		m_slots.push_back(m_referrer->items.get(id));
		char c;
		ser >> c;
		if (c == ';')
		{
		}
		else if (c == '+')
		{
			std::shared_ptr<Metadata> data;
			ser >> *data;
			m_metadata.emplace(i, data);
		}
	}
}
