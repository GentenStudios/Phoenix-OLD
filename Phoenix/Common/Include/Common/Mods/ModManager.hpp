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

#include <Common/Mods/Mod.hpp>
#include <Common/Mods/ModAPI.hpp>

#include <functional>
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
		explicit ModManager(const ModList& modList);

		// using just typename F is not particularly safe in terms of checking
		// for safe use, however, it works, using something like:
		//		template<typename RtnType, typename... Args>
		//		void registerFunction(const std::string& funcName,
		//							std::function<RtnType(Args...)> func);
		// doesn't actually compile. :(
		template<typename F>
		void registerFunction(const std::string& funcName, F func);

		// you can make status return a percentage for a progress bar.
		Status load(float* progress);
		void   cleanup();

		const ModList&     getModList() const;
		const Privileges*  getPrivileges() const;
		const CommandBook* getCommandBook() const;

		sol::state m_luaState;
	private:
		struct InternalTree
		{
			std::string               branch;
			std::vector<InternalTree> twigs;
		};

		std::vector<InternalTree> m_functionTree;

		std::vector<std::string> m_modsRequired;
		std::vector<Mod>         m_mods;

		Privileges  m_privileges;
		CommandBook m_commandBook;

	};
} // namespace phx::mods
