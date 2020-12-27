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

#include <Common/CMS/ModManager.hpp>
#include <Common/CMS/Mod.hpp>
#include <Common/Logger.hpp>
#include <Common/Math/Math.hpp>

#include <fstream>
#include <queue>

using namespace phx::cms;

static void CustomPanicHandler(sol::optional<std::string> maybe_msg)
{
	// \n for newline, \t for tab.
	std::string error;
	if (maybe_msg)
	{
		error += "\n\t";
		error += maybe_msg.value();
		error += "\n";
	}

	LOG_FATAL("MODDING") << "An unexpected Lua error has occured. " << error
	                     << "The application will now be aborted.";
}

ModManager::ModManager(const ModList& toLoad, const ModList& paths)
    : m_modsRequired(toLoad), m_modPaths(paths)
{
	m_luaState.open_libraries(sol::lib::base);
	m_luaState.set_panic(
	    sol::c_call<decltype(&CustomPanicHandler), &CustomPanicHandler>);
}

ModManager::Status ModManager::load(float* progress)
{
	std::queue<Mod> toLoad;

	for (auto& require : m_modsRequired)
	{
		bool found = false;
		for (auto& path : m_modPaths)
		{
			std::fstream file;
			file.open(path + "/" + require + "/Init.lua");
			if (file.is_open())
			{
				found = true;
				toLoad.emplace(require, path);
				break;
			}
		}

		if (!found)
		{
			std::string pathList = "\n\t";
			for (auto& path : m_modPaths)
			{
				pathList += path;
				pathList += "\n\t";
			}

			LOG_FATAL("MODDING")
			    << "The mod: " << require
			    << " was not found in any of the provided mod directories: "
			    << pathList;

			return {false, "The mod: " + require + " was not found."};
		}
	}

	std::vector<std::string> loadedMods;
	while (!toLoad.empty())
	{
		std::size_t lastPass = toLoad.size();

		for (std::size_t i = 0; i < toLoad.size(); ++i)
		{
			Mod& mod       = toLoad.front();
			bool satisfied = true;

			for (auto& dependency : mod.getDependencies())
			{
				if (std::find(loadedMods.begin(), loadedMods.end(),
				              dependency) == loadedMods.end())
				{
					satisfied = false;
				}
			}

			if (satisfied)
			{
				m_currentModPath = mod.getPath() + "/" + mod.getName() + "/";
				sol::protected_function_result pfr =
				    m_luaState.safe_script_file(m_currentModPath + "Init.lua",
				                                &sol::script_pass_on_error);

				loadedMods.push_back(mod.getName());

				// error occured if return is not valid.
				if (!pfr.valid())
				{
					sol::error err = pfr; // visual studio marks this as an
					                      // error, but it's fine.

					std::string errString = "An error occured loading ";
					errString += mod.getName();
					errString += ": ";
					errString += err.what();

					LOG_FATAL("MODDING") << errString;

					return {false, errString};
				}
			}
			else
			{
				toLoad.push(toLoad.front());
			}

			toLoad.pop();
		}

		if (lastPass == toLoad.size())
		{
			std::string err = "The mod: ";
			err += toLoad.front().getName();
			err += " is missing one or more dependencies, please resolve this "
			       "issue before continuing.";

			LOG_FATAL("MODDING") << err;

			return {false, err};
		}
	}

	// no need to put in something for "what", since nothing went wrong.
	return {true};
}

void ModManager::cleanup() {}

const ModManager::ModList& ModManager::getModList() const
{
	return m_modsRequired;
}

const std::string& ModManager::getCurrentModPath() const
{
	return m_currentModPath;
}
