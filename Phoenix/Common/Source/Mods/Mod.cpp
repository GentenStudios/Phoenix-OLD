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
#include <Common/Mods/Mod.hpp>

#include <fstream>

using namespace phx::mods;

Mod::Mod(const std::string& modName, const std::string& modFolder)
{
	m_name      = modName;
	m_modFolder = modFolder;

	{
		std::fstream deps;

		deps.open(modFolder + "/" + modName + "/Dependencies.txt");
		if (!deps.is_open())
		{
			LOG_WARNING("[MOD]")
			    << "Mod " << modName << " does not include a Dependencies.txt!";

			// assuming no deps, even if it causes failure further down.
			return;
		}

		std::string input;
		while (std::getline(deps, input))
		{
			m_dependencies.push_back(input);
		}

		deps.close();
	}
}

const Mod::Dependencies& Mod::getDependencies() const { return m_dependencies; }

void Mod::load(sol::state_view lua)
{
	const std::string filename = m_modFolder + "/" + m_name + "/Init.lua";
	auto              result = lua.safe_script_file(filename);
}

const std::string& Mod::getName() const { return m_name; }
