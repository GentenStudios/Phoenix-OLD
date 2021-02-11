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

// Header exists in ModManager.hpp

namespace phx::cms
{
	// can do template<typename RtnType, typename... Args>
	// but allowing for anything makes sure you can capture lambdas.
	template <typename F>
	void ModManager::registerFunction(const std::string& funcName, const F& func)
	{
		// splitting the function name by periods.
		// something like core.block.register would be broken down into
		// core, block and register. core and block need to be created as
		// Lua tables, however register is just set to the function since
		// it's the actual name of it.
		std::vector<std::string> branches;
		std::stringstream        sstream(funcName);
		std::string              substr;
		while (std::getline(sstream, substr, '.'))
		{
			branches.push_back(substr);
		}

		// malformed request.
		if (branches.empty())
			return;

		// limiting to 3 levels of hierarchy for a v1. Otherwise deep
		// infinite recursion doesn't work.
		if (branches.size() == 1)
		{
			m_luaState[branches[0]] = func;
			return;
		}

		if (branches.size() == 2)
		{
			if (!m_luaState[branches[0]].valid())
			{
				m_luaState[branches[0]] = m_luaState.create_table();
			}

			m_luaState[branches[0]][branches[1]] = func;
		}

		if (branches.size() == 3)
		{
			if (!m_luaState[branches[0]].valid())
			{
				m_luaState[branches[0]] = m_luaState.create_table();
			}

			if (!m_luaState[branches[0]][branches[1]].valid())
			{
				m_luaState[branches[0]][branches[1]] =
				    m_luaState.create_table();
			}

			m_luaState[branches[0]][branches[1]][branches[2]] = func;
		}

		if (branches.size() > 3)
		{
			LOG_WARNING("MODDING")
			    << "Function hierarchies of more than 3 are not currently "
			       "supported. Please limit function names to the depth of "
			       "\"core.block.register\".";
		}
	}

	template <typename T, typename... Args>
	sol::usertype<T> ModManager::registerType(std::string name,
	                                          sol::constructors<Args...>)
	{
		return m_luaState.new_usertype<T>(name, sol::constructors<Args...>());
	}

} // namespace phx::cms
