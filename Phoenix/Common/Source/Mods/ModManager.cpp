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

using namespace phx::mods;

ModManager::ModManager(const ModList& modList) : m_modsRequired(modList)
{
	m_luaState.open_libraries(sol::lib::base);
}

// template <typename RtnType, typename... Args>
void ModManager::registerFunction(const std::string& funcName)
{
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

	std::vector<InternalTree>::iterator it =
	    std::find_if(m_functionTree.begin(), m_functionTree.end(),
	                 [branches](const InternalTree& branch) {
		                 if (branch.branch == branches[0])
		                 {
			                 return true;
		                 }

		                 return false;
	                 });

	if (it == m_functionTree.end())
	{
		it = m_functionTree.insert(m_functionTree.end(), {branches[0], {}});
		m_luaState[branches[0]] = m_luaState.create_table();
	}

	InternalTree& currentTree = *it;
	auto         currentPos  = m_luaState[branches[0]];

	// we don't process the last one, that's the function name.
	// we've already checked for the first one too, that's the base, like
	// "core".
	for (int i = 1; i <= branches.size() - 1; ++i)
	{
		auto exist =
		    std::find_if(currentTree.twigs.begin(), currentTree.twigs.end(),
		                 [currentTree, branches, i](const InternalTree& tree) {
			                 if (branches[i] == tree.branch)
			                 {
				                 return true;
			                 }

			                 return false;
		                 });

		if (exist == currentTree.twigs.end())
		{
			exist = currentTree.twigs.insert(currentTree.twigs.end(),
			                                 {branches[i], {}});
			currentPos[branches[i]] = m_luaState.create_table();
		}

		// vyom this is your issue fix tomoz.
		// something to do with it being a reference so we fucked up...
		currentPos  = currentPos[branches[i]];
		currentTree = *exist;
	}

	LOG_FATAL("[FUNC]") << branches.size();
	LOG_FATAL("[FUNC]") << m_functionTree[0].branch
	                    << m_functionTree[0].twigs[0].branch;
	//	                    << m_functionTree[0].twigs[0].twigs[0].branch;
}

ModManager::Status ModManager::load(float* progress) { return {true}; }

void ModManager::cleanup() {}

const ModManager::ModList& ModManager::getModList() const
{
	return m_modsRequired;
}

const Privileges* ModManager::getPrivileges() const { return &m_privileges; }

const CommandBook* ModManager::getCommandBook() const { return &m_commandBook; }
