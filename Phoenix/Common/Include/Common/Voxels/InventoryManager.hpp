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

#include <Common/Math/Math.hpp>
#include <Common/Save.hpp>
#include <Common/Utility/BlockingQueue.hpp>
#include <Common/Voxels/Inventory.hpp>
#include <Common/Voxels/ItemReferrer.hpp>

#include <cstddef>
#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace phx::voxels
{
	class InventoryManager
	{
	public:
		InventoryManager(Save* save, voxels::ItemReferrer* referrer);

		/**
		 * @brief Gets an inventory from storage.
		 * @param index The index of the inventory.
		 * @return The inventory or a nullptr if one doesn't exist.
		 */
		Inventory*  getInventory(std::size_t index);
		std::size_t createInventory(std::size_t size);
		void        eraseInventory(std::size_t index);

		void save(std::size_t index);

	private:
		bool load(std::size_t index);

		std::filesystem::path toSavePath(std::size_t index);

	private:
		ItemReferrer* m_referrer;
		Save*         m_save = nullptr;

		std::unordered_map<std::size_t, Inventory> m_inventories;
		std::queue<std::size_t>                    m_deletedSlots;
	};
} // namespace phx::voxels