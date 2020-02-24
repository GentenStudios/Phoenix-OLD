// Copyright 2020 Genten Studios
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

/**
 * @file ContentLoader.hpp
 * @brief Implements Lua module loading system.
 *
 * @copyright Copyright (c) 2020 Genten Studios
 *
 */

#pragma once

#include <Common/Singleton.hpp>

#include <sol/sol.hpp>

#include <vector>
#include <string>

namespace phx
{
	/// @brief A lightweight struct to store a module and the list of
	/// dependencies that module has
	struct Mod
	{
		/// @brief The name of the module, should match the folder name
		std::string name;
		/// @brief List of dependencies that the module needs inorder to load
		std::vector<std::string> dependencies;

		/**
		 * @brief Constructor for mod object, folder matching mod name with a
		 * dependencies.txt inside modules folder must exist
		 */
		explicit Mod(std::string name);
		~Mod() = default;
	};

    class ContentManager : public Singleton<ContentManager>
	{
	public:
		/**
		 * @brief Loads necessary lua modules required to load a save file
		 *
		 * @param save The save file to be loaded
		 * @param lua The sol state used during runtime
		 * @return true If the function successfully loaded all modules
		 * @return false If the function failed to load modules, details will
		 *         be outputted to the terminal
		 *
		 */
		bool loadModules(const std::string& save);
		//TODO : Add proper error handling instead of returning a boolean

		sol::state lua;

		std::string currentMod;
		
		ContentManager();
	};
}; // namespace q2
