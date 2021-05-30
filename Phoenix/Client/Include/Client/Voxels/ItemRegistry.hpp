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

#include <Common/CMS/ModManager.hpp>
#include <Common/Registry.hpp>
#include <Common/Voxels/ItemReferrer.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace phx::client
{
	/**
	 * @brief Acts as a register for item data throughout the client
	 * application.
	 *
	 * This holds a block referrer which handles default initialized blocks and
	 * then provides an API registration for registering blocks from within Lua.
	 */
	struct ItemRegistry
	{
		voxels::ItemReferrer referrer;

		void registerAPI(cms::ModManager* manager)
		{
			manager->registerFunction(
			    "voxel.item.register", [manager, this](sol::table luaItem) {
				    voxels::ItemType item;

				    sol::optional<std::string> name = luaItem["name"];
				    if (name)
				    {
					    item.displayName = *name;
				    }
				    else
				    {
					    // log the error and return to make this a recoverable
					    // error.
					    LOG_FATAL("MODDING")
					        << "The mod at: " << manager->getCurrentModPath()
					        << " attempted to register an item without "
					        << "specifying a name.";
					    return;
				    }

				    sol::optional<std::string> id = luaItem["id"];
				    if (id)
				    {
					    item.id = luaItem.get<std::string>("id");
				    }
				    else
				    {
					    // log the error and return to make this a recoverable
					    // error.
					    LOG_FATAL("MODDING")
					        << "The mod at: " << manager->getCurrentModPath()
					        << " attempted to register an item ("
					        << item.displayName
					        << ") without specifying an id.";
					    return;
				    }

				    sol::optional<size_t> maxStack = luaItem["max_stack"];
				    if (maxStack)
				    {
					    item.maxStack = *maxStack;
				    }

				    sol::optional<std::string> places = luaItem["places"];
				    if (places)
				    {
					    item.places = *places;
				    }

				    sol::optional<sol::function> onPrimary =
				        luaItem["onPrimary"];
				    if (onPrimary)
				    {
					    item.onPrimary = *onPrimary;
				    }

				    sol::optional<sol::function> onSecondary =
				        luaItem["onSecondary"];
				    if (onSecondary)
				    {
					    item.onSecondary = *onSecondary;
				    }

				    item.uid = referrer.referrer.size();
				    referrer.referrer.add(item.id, item.uid);
				    referrer.items.add(item.uid, item);
			    });
		}
	};
} // namespace phx::client