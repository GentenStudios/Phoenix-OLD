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

#include <sol/sol.hpp>

#include <string>
#include <vector>

namespace phx::cms
{
	/**
	 * @brief Class to encapsulate a "mod".
	 *
	 * This class is literally just to load dependencies and provide an easy way
	 * to get the name and path of the mod. It's tidier to store a vector of
	 * mods than a vector of names, paths, and dependencies.
	 */
	class Mod
	{
	public:
		using Dependencies = std::vector<std::string>;

	public:
		Mod() = delete;

		/**
		 * @brief Constructs a mod object and reads the dependency list.
		 * @param modName The name of the mod.
		 * @param modPath The path that the mod is found in.
		 *
		 * Mod Path should be the path that the mod is found in, not the
		 * directory of the mod itself.
		 */
		Mod(const std::string& modName, const std::string& modPath);

		/**
		 * @brief Gets the name of the mod.
		 * @return The name of the mod.
		 */
		const std::string&  getName() const;

		/**
		 * @brief Gets the parent folder of the mod.
		 * @return The path that the mod resides in.
		 *
		 * It will return the folder that the mod resides in, not the mod's
		 * folder itself. Combine with getName() to make a whole path.
		 */
		const std::string&  getPath() const;

		/**
		 * @brief Gets the mod's dependencies.
		 * @return An array of dependencies for the mod.
		 */
		const Dependencies& getDependencies() const;

	private:
		std::string m_name;
		std::string m_path;

		Dependencies m_dependencies;
	};
} // namespace phx::mods
