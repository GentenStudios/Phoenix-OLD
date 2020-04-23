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

#include <Common/Logger.hpp>
#include <Common/Mods/ModManager.hpp>

#include <type_traits>

using namespace phx::mods;

ModManager::ModManager(const ModList& modList) : m_modsRequired(modList)
{
	m_luaState.open_libraries(sol::lib::base);
}

template <typename F>
void ModManager::registerFunction(const std::string& funcName, F func)
{
	// splitting the function name by periods.
	// something like core.block.register would be broken down into core, block
	// and register. core and block need to be created as Lua tables, however
	// register is just set to the function since it's the actual name of it.
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

	// limiting to 3 levels of hierarchy for a v1. Otherwise deep infinite
	// recursion doesn't work.
	if (branches.size() == 1)
	{
		m_luaState[branches[0]] = []() { return -1; };
		return;
	}

	if (branches.size() == 2)
	{
		if (!m_luaState[branches[0]].valid())
		{
			m_luaState[branches[0]] = m_luaState.create_table();
		}

		m_luaState[branches[0]][branches[1]] = []() { return -1; };
	}

	if (branches.size() == 3)
	{
		if (!m_luaState[branches[0]].valid())
		{
			m_luaState[branches[0]] = m_luaState.create_table();
		}

		if (!m_luaState[branches[0]][branches[1]].valid())
		{
			m_luaState[branches[0]][branches[1]] = m_luaState.create_table();
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

	// keeping for future use.
	//// recursively checks whether the tables for the function exist and just
	//// pushes the function onto the table.
	// std::vector<sol::table> tables;
	// tables.resize(branches.size() - 1);
	// for (std::size_t i = branches.size() - 1; i > 0; --i)
	//{
	//	tables[i - 1] = m_luaState.create_table();

	//	if (i == branches.size() - 1)
	//	{
	//		// replace wid actual function
	//		tables[i - 1][branches[i]] = []() { return -1; };
	//	}

	//	if (i < branches.size() - 1)
	//	{
	//		tables[i - 1][branches[i]] = tables[i];
	//	}
	//}

	// m_luaState[branches[0]] = tables[0];
}

ModManager::Status ModManager::load(float* progress) { return {true}; }

void ModManager::cleanup() {}

const ModManager::ModList& ModManager::getModList() const
{
	return m_modsRequired;
}

const Privileges* ModManager::getPrivileges() const { return &m_privileges; }

const CommandBook* ModManager::getCommandBook() const { return &m_commandBook; }
