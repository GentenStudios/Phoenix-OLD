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
		m_stacks.emplace_back(0);
	}
}

Item Inventory::getItem(std::size_t slot)
{
	if (m_slots.size() >= slot)
	{
		Item item({m_slots[slot], nullptr, m_stacks[slot]});
		if (m_metadata.find(slot) != m_metadata.end())
		{
			item.metadata = m_metadata[slot];
		}
		return item;
	}
	return {m_referrer->items.get(ItemType::INVALID_ITEM), nullptr, 0};
}

bool Inventory::addItem(std::size_t slot, Item& item)
{
	if (m_size <= slot)
	{
		LOG_DEBUG("Inventory")
		    << "Attempted to insert item into non-existent slot";
		return false;
	}
	if (getItem(slot).type == nullptr)
	{
		m_slots[slot] = item.type;
		if (item.metadata != nullptr)
		{
			m_metadata[slot] = item.metadata;
		}
		m_stacks[slot] = item.volume;
		item.volume    = 0;
		return true;
	}
	if (m_slots.at(slot) == item.type &&
	    m_stacks.at(slot) < item.type->maxStack)
	{
		if (m_metadata.find(slot) != m_metadata.end() ||
		    item.metadata != nullptr)
		{
			LOG_DEBUG("Inventory") << "Attempted to stack items with metadata";
			return false;
		}
		if (m_stacks[slot] + item.volume > item.type->maxStack)
		{
			item.volume    = item.volume + m_stacks[slot] - item.type->maxStack;
			m_stacks[slot] = item.type->maxStack;
			return false;
		}
		m_stacks[slot] += item.volume;
		item.volume = 0;
		return true;
	}
	LOG_DEBUG("Inventory") << "Attempted to insert item into occupied slot";
	return false;
}

int Inventory::addItem(Item& item)
{
	for (std::size_t i = 0; i < m_size; i++)
	{
		if (m_slots.at(i) == nullptr || m_slots.at(i) == item.type)
		{
			if (addItem(i, item))
			{
				return i;
			}
		}
	}
	return -1;
}

Item Inventory::removeItem(std::size_t slot, bool all)
{
	if (m_slots.size() >= slot)
	{
		Item item;
		item.type = m_slots.at(slot);
		if (all)
		{
			item.volume       = m_stacks.at(slot);
			m_stacks.at(slot) = 0;
			m_slots.at(slot)  = nullptr;
		}
		else
		{
			item.volume = 1;
			m_stacks.at(slot)--;
			if (m_stacks.at(slot) == 0)
			{
				m_slots.at(slot) = nullptr;
			}
		}
		if (m_metadata.find(slot) != m_metadata.end())
		{
			item.metadata = m_metadata[slot];
			m_metadata.erase(slot);
		}
		return item;
	}
	return {m_referrer->items.get(ItemType::INVALID_ITEM), nullptr, 0};
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
		LOG_DEBUG("Inventory")
		    << "Attempted to set metadata for out of bounds inventory slot";
		return false;
	}
	if (m_slots.at(slot) == nullptr)
	{
		LOG_DEBUG("Inventory")
		    << "Attempted to set metadata for empty inventory slot";
		return false;
	}
	if (m_stacks.at(slot) > 1)
	{
		LOG_DEBUG("Inventory") << "Attempted to set metadata a stack of items";
		return false;
	}
	return m_metadata[slot]->set(key, newData);
}

phx::Serializer& Inventory::operator>>(phx::Serializer& ser) const
{
	ser << m_size;
	for (std::size_t i = 0; i < m_size; i++)
	{
		ser << m_slots[i]->uniqueIdentifier;
		if (m_metadata.find(i) != m_metadata.end())
		{
			ser << '+' << *m_metadata.at(i);
		}
		else if (m_stacks.at(i) > 1)
		{
			ser << '*' << m_stacks.at(i);
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
		m_stacks.at(i) = 1;
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
		else if (c == '*')
		{
			std::size_t volume;
			ser >> volume;
			m_stacks.at(i) = volume;
		}
	}
	return ser;
}
