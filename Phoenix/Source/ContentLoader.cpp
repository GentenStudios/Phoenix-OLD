// Copyright 2019 Genten Studios
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

#include <fstream>
#include <iostream>
#include <queue>

#include <Phoenix/ContentLoader.hpp>

using namespace phx;

std::string ContentManager::m_currentMod = "";

Mod::Mod(std::string modName) : name(std::move(modName))
{
	std::fstream fileStream;
	fileStream.open("Modules/" + name + "/Dependencies.txt");
	if (!fileStream.is_open())
	{
		std::cout << "Couldnt find dependencies file for mod: " << name << "\n";
		return;
	}
	while (fileStream.peek() != EOF)
	{
		std::string input;
		std::getline(fileStream, input);
		dependencies.push_back(input);
	}
	fileStream.close();
};

bool ContentManager::loadModules(const std::string& save, sol::state& lua)
{
	std::fstream    fileStream;
	std::queue<Mod> toLoad; // A queue of mods that need loaded

	fileStream.open("Save/" + save + "/Mods.txt");
	if (!fileStream.is_open())
	{
		std::cout << "Error opening save file";
		return false;
	}
	int i = 0;
	while (fileStream.peek() != EOF)
	{
		std::string input;
		std::getline(fileStream, input);
		Mod mod = Mod(input);
		toLoad.push(mod);
		if (i > 10)
			break;
		i++;
	}
	fileStream.close();

	// Sort and load the mods
	std::vector<std::string> loadedMods;
	while (!toLoad.empty())
	{
		int lastPass = toLoad.size();

		// For each mod that needs loaded
		for (int i = 0; i < toLoad.size(); i++)
		{
			Mod  mod       = toLoad.front();
			bool satisfied = true;

			// For each dependency the mod has
			for (int j = 0; j < mod.dependencies.size(); j++)
			{
				// If dependency is not satisfied, mark satisfied as false.
				if (std::find(loadedMods.begin(), loadedMods.end(),
				              mod.dependencies[j]) == loadedMods.end())
				{
					satisfied = false;
				}
			}

			// If all dependencies were met, run lua and add mod to satisfied
			// list Otherwise, move mod to back of load queue
			if (satisfied)
			{
				m_currentMod = mod.name;
				lua.script_file("Modules/" + mod.name + "/Init.lua");
				loadedMods.push_back(mod.name);
			}
			else
			{
				toLoad.push(toLoad.front());
			}
			toLoad.pop();
		}

		// If we haven't loaded any new mods, throw error
		if (lastPass == toLoad.size())
		{
			// TODO: Replace this with actuall error handling/ logging
			std::cout
			    << "One or more mods are missing required dependencies. \n";
			std::cout << "Failed Mods:\n";
			for (int i = 0; i < toLoad.size(); i++)
			{
				std::cout << "- " + toLoad.front().name + "\n";
				toLoad.pop();
			}
			std::cout << "Loaded Mods:\n";
			for (int i = 0; i < loadedMods.size(); i++)
			{
				std::cout << "- " + loadedMods[i] + "\n";
			}
			return false;
		}
	}
	return true;
}

// TODO: replace this with an API registration system
#include <Phoenix/Commander.hpp>
#include <Phoenix/Settings.hpp>
#include <Phoenix/Voxels/BlockRegistry.hpp>
#include <array>
#include <map>

void ContentManager::loadAPI(sol::state& lua, ImGui::BasicTerminal& chat)
{
	/**
	 * @defgroup luaapi Lua API
	 *
	 * The lua API is . . .
	 *
	 * @tableofcontents
	 *
	 */
	lua["core"] = lua.create_table();
	/**
	 * @addtogroup luaapi
	 *
	 * ---
	 * ---
	 * @section core core
	 * @brief The core API for interacting with Quartz
	 */
	lua["core"]["print"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreprint core.print(text)
	     * @brief Prints text to the players terminal
	     *
	     * @param text The text to be outputted to the terminal
	     *
	     */
	    [&chat](std::string text) { chat.cout << text << "\n"; };
	lua["core"]["setting"] = lua.create_table();
	/**
	 * @addtogroup luaapi
	 *
	 * ---
	 * @subsection coreset core.setting
	 * @brief Interfaces with the settings system
	 */
	lua["core"]["setting"]["register"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coresetreg core.setting.register(displayName, key,
	     * defaultValue)
	     * @brief Registers a setting that the player can adjust via the
	     * settings menu
	     *
	     * @param displayName The Display name for the setting seen in the
	     * settings menu
	     * @param key The unique key for the setting, usually in the form
	     * module:setting
	     * @param defaultValue The default value for the setting if not already
	     * set
	     *
	     */
	    [](std::string displayName, std::string key, int defaultValue) {
		    Settings::get()->add(displayName, key, defaultValue);
	    };
	lua["core"]["setting"]["get"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coresetget core.setting.get(key)
	     * @brief Gets the value of a setting based on its unique key
	     *
	     * @param key The unique key for the setting, usually in the form
	     * module:setting
	     * @return The integer value of the setting
	     *
	     */
	    [](std::string key) {
		    return Settings::get()->getSetting(key)->value();
	    };
	lua["core"]["setting"]["set"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coresetset core.setting.set(key)
	     * @brief Sets the value of a setting based on its unique key
	     *
	     * @param key The unique key for the setting, usually in the form
	     * module:setting
	     * @param value The value the setting should be set to
	     *
	     */
	    [](std::string key, int value) {
		    Settings::get()->getSetting(key)->set(value);
	    };
	lua["core"]["command"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * ---
	     * @subsection corecmd core.command
	     * @brief Interfaces with the commander
	     *
	     */
	    lua.create_table();
	lua["core"]["command"]["register"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection corecmdreg core.command.register
	     * @brief Registers a new command
	     *
	     * In the terminal typing "/" followed by a command will execute the
	     *command
	     *
	     * @param command The command to register
	     * @param help A helpstring that is printed to terminal when typing
	     *"/help <command>"
	     * @param f The callback function that is called by the commander
	     * The callback function must take a table as an argument
	     * Any arguments included when the command is executed will be passed in
	     *this table
	     *
	     * @b Example:
	     * @code {.lua}
	     * function hello (args)
	     *     if args[1] == "there" then
	     *         print("General Kenobi")
	     *	   elseif args[1] == "world" then
	     * 		   print("World says hi")
	     *	   else
	     *         print("with you, the force is not")
	     *     end
	     * end
	     * core.command.register("Hello", "Master the arts of the Jedi you
	     *must", hello)
	     * @endcode
	     */
	    [](std::string command, std::string help, sol::function f) {
		    CommandBook::get()->add(command, help, "all", f);
	    };
	lua["voxel"] = lua.create_table();
	/**
	 * @addtogroup luaapi
	 *
	 * ---
	 * ---
	 * @section voxel voxel
	 * @brief The voxel API for interacting voxels
	 */
	lua["voxel"]["block"] = lua.create_table();
	/**
	 * @addtogroup luaapi
	 *
	 * ---
	 * @subsection voxelblock voxel.block
	 * @brief Interfaces with blocks
	 */
	lua["voxel"]["block"]["register"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection voxelblockreg voxel.block.register(luaBlock)
	     * @brief Sets the value of a setting based on its unique key
	     *
	     * @param luaBlock a table storing the data used to create a block
	     *
	     * @details The luaBlock table stores the following values: \n
	     * Required:
	     * - @b name: The display name for the block
	     * - @b id: The unique id for the block usually in the form module:block
	     *
	     * Optional:
	     * - @b category: The category of the block chosen from "Air", "Liquid",
	     * or "Solid" \n If not specified, the default is "Solid"
	     * - @b onPlace: A function that is run when the block is placed
	     * - @b onBreak: A function that is run when the block is broken
	     * - @b onInteract: A function that is run when the block is interacted
	     * with
	     * - @b textures: A table of filepaths where textures are located \n
	     * 			   Filepaths are relative to to the module directory \n
	     * 			   If only one filepath is specified, that will be used for all
	     * textures
	     *
	     * @b Example:
	     *
	     * @code {.lua}
	     * block = {}
	     * block.name = "Grass"
	     * block.id = "core:grass"
	     * block.textures = {"Assets/grass_side.png", "Assets/grass_side.png",
	     * "Assets/grass_side.png", "Assets/grass_side.png",
	     * "Assets/grass_top.png",  "Assets/dirt.png"}
	     * block.onBreak = function (position)
	     * 	print("grass broken at" + position)
	     * end
	     * voxel.block.register(block)
	     * @endcode
	     */
	    [](sol::table luaBlock) {
		    using namespace phx::voxels;
		    BlockType block;
		    {
			    block.displayName = luaBlock["name"];
			    block.id          = luaBlock["id"];

			    if (luaBlock["category"] == "Air")
			    {
				    block.category = BlockCategory::AIR;
				    std::cout << "air" << std::endl;
			    }
			    else if (luaBlock["category"] == "Liquid")
			    {
				    block.category = BlockCategory::LIQUID;
			    }
			    else
			    {
				    block.category = BlockCategory::SOLID;
			    }

			    if (luaBlock["onPlace"])
			    {
				    block.onPlace = luaBlock["onPlace"];
			    }

			    if (luaBlock["onBreak"])
			    {
				    block.onPlace = luaBlock["onPlace"];
			    }

			    if (luaBlock["onInteract"])
			    {
				    block.onPlace = luaBlock["onPlace"];
			    }

			    std::array<std::string, 6> textures;
			    if (block.category != BlockCategory::AIR)
			    {
				    for (int i = 0; i < 6; i++)
				    {
					    std::string texture = luaBlock["textures"][i + 1];
					    if (texture.empty())
					    {
						    // If a texture is not supplied, we use the first
						    // texture in its place
						    texture = luaBlock["textures"][1];
					    }
					    textures[i] = "Modules/" + m_currentMod + "/" + texture;
				    }
			    }
			    block.textures = textures;
		    }
		    BlockRegistry::get()->registerBlock(block);
	    };
}