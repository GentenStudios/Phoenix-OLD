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

#include <Common/Logger.hpp>

#include <sol/sol.hpp>

#include <functional>
#include <queue>
#include <vector>

namespace phx::mods
{
	class ModManager
	{
	public:
		struct Status
		{
			bool        ok;
			std::string what;
		};

		using ModList = std::vector<std::string>;

	public:
		ModManager() = delete;

		// mod paths is a list of paths where mods could be stored.
		// mod list is literally just the list of mods to load.
		explicit ModManager(const ModList& toLoad, const ModList& paths);

		template <typename F>
		void registerFunction(const std::string& funcName, F func);

		template <typename T>
		void exposeVariable(const std::string& name, T var);

		// you can make status return a percentage for a progress bar.
		Status load(float* progress);
		void   cleanup();

		const ModList&     getModList() const;
		const std::string& getCurrentModPath() const;

	private:
		std::vector<std::string> m_modsRequired;
		std::vector<std::string> m_modPaths;
		std::string              m_currentModPath;

		sol::state m_luaState;
	};
} // namespace phx::mods

#include "ModManager.inl"
