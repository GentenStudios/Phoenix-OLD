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

#include "ItemReferrer.hpp"
#include <Common/Voxels/Item.hpp>

namespace phx::voxels
{
	class Inventory : public ISerializable
	{
	public:
		/**
		 * Create an inventory for storing items.
		 * @param size The size of the inventory.
		 * @param referrer The ItemReferrer used for this instance of the game.
		 */
		Inventory(std::size_t size, ItemReferrer* referrer);

		/**
		 * Get an item from the inventory without removing the item.
		 * @param slot The slot the item is in.
		 * @return The item or nullptr if the slot is empty.
		 */
		Item getItem(std::size_t slot);

		/**
		 * Add an item to the inventory.
		 * @param slot The slot you want to insert the item into.
		 * @param item The item you want to insert into the inventory.
		 * @return true If the item was inserted.
		 * @return false If the item was not inserted.
		 */
		bool addItem(std::size_t slot, const Item& item);

		/**
		 * Add an item to the next open slot in the inventory.
		 * @param item The item you want to insert into the inventory.
		 * @return The index of the item slot if the item was inserted.
		 * @return -1 If the item was not inserted.
		 */
		int addItem(const Item& item);

		/**
		 * Removes an item from the inventory.
		 * @param slot The slot the item is in.
		 * @return The item or nullptr if the slot is empty.
		 */
		Item removeItem(std::size_t slot);

		/**
		 * @brief Sets metadata for the Item at the supplied slot.
		 * @param slot The slot the item is in.
		 * @param key The key associated with the metadata.
		 * @param newData The new value for the data.
		 * @return true if the data was set.
		 * @return false if the data already exists with a different data type
		 * OR if the supplied slot is out of bounds for the inventory.
		 */
		bool setMetadataAt(std::size_t slot, const std::string& key,
		                   std::any* newData);

		const std::vector<ItemType*>& getItems() const { return m_slots; };
		size_t                        getSize() const { return m_size; };

		// serialize.
		Serializer& operator>>(Serializer& ser) const override;

		// deserialize.
		Serializer& operator<<(Serializer& ser) override;

	private:
		size_t                 m_size;
		std::vector<ItemType*> m_slots;
		Metadata::Container    m_metadata;
		ItemReferrer*          m_referrer;
	};
} // namespace phx::voxels
