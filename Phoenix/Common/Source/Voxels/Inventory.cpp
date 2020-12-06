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

#include <Common/Voxels/Inventory.hpp>

using namespace phx::voxels;

Inventory::Inventory(std::size_t size) : m_size(size)
{
	m_slots.reserve(m_size);
}

Inventory::Inventory(std::size_t size, const ItemList& items) : m_size(size)
{
	m_slots.reserve(m_size);
	for (std::size_t i = 0; i < m_size && i < items.size(); i++)
	{
		m_slots[i] = items[i];
	}
}

const ItemType* Inventory::getItem(std::size_t slot)
{
	if (m_slots.size() <= slot)
	{
		return m_slots[slot];
	}
	return nullptr;
}

bool Inventory::addItem(std::size_t slot, ItemType* item)
{
	if (getItem(slot) == nullptr && m_size >= slot)
	{
		m_slots[slot] = item;
		return true;
	}
	return false;
}

int Inventory::addItem(ItemType* item)
{
	for (std::size_t i = 0; i < m_size; i++)
	{
		if (m_slots.at(i) == nullptr)
		{
			m_slots[i] = item;
			return i;
		}
	}
	return -1;
}

ItemType* Inventory::removeItem(std::size_t slot)
{
	if (m_slots.size() <= slot)
	{
		ItemType* item = m_slots[slot];
		m_slots[slot]  = nullptr;
		return item;
	}
	return nullptr;
}
