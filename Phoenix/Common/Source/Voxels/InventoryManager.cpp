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

#include <Common/Voxels/InventoryManager.hpp>

#include <fstream>

using namespace phx::voxels;
using namespace phx;

InventoryManager::InventoryManager(Save* save, voxels::ItemReferrer* referrer)
    : m_referrer(referrer), m_save(save)
{
}

Inventory* InventoryManager::getInventory(std::size_t index)
{
	if (m_inventories.find(index) != m_inventories.end())
	{
		return &m_inventories.at(index);
	}
	return nullptr;
}

std::size_t InventoryManager::createInventory(std::size_t size)
{
	if (m_deletedSlots.empty())
	{
		for (std::size_t i = 0; i < m_inventories.max_size(); i++)
		{
			if (m_inventories.find(i) == m_inventories.end())
			{
				m_inventories.emplace(i, Inventory(size, m_referrer));
				return i;
			}
		}
		// TODO WTF do we do if we actually manage to max this out?
	}
	else
	{
		std::size_t i = m_deletedSlots.front();
		m_deletedSlots.pop();
		m_inventories.emplace(i, Inventory(size, m_referrer));
		return i;
	}
}

void InventoryManager::eraseInventory(std::size_t index)
{
	if (m_inventories.find(index) != m_inventories.end())
	{
		m_inventories.erase(index);
		m_deletedSlots.push(index);
	}
}

void InventoryManager::save(const std::size_t index)
{
	if (m_inventories.find(index) != m_inventories.end())
	{
		std::ofstream saveFile(toSavePath(index), std::ofstream::binary);

		Serializer ser;
		ser << m_inventories.at(index);
		saveFile.write((char*) &ser.getBuffer()[0], ser.getBuffer().size());

		saveFile.close();
	}
}

bool InventoryManager::load(std::size_t index)
{
	std::ifstream saveFile(toSavePath(index), std::ifstream::binary);

	if (!saveFile)
	{
		return false;
	}

	saveFile.seekg(0, std::ifstream::end);
	int length = saveFile.tellg();
	saveFile.seekg(0, std::ifstream::beg);

	Serializer ser;
	data::Data data(length);
	saveFile.read((char*) &data[0], length);
	ser.setBuffer(data);

	Inventory inventory = Inventory(0, m_referrer);
	ser >> inventory;
	m_inventories.emplace(index, std::move(inventory));
	return true;
}
std::filesystem::path InventoryManager::toSavePath(std::size_t index)
{
	// TODO This probably should be chunked to n inventories per save file
	return std::filesystem::path(phx::saveDir + m_save->getName() +
	                             "/inventories/" + std::to_string(index) +
	                             ".save");
}
