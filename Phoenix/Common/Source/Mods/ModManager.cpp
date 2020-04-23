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

#include <Common/Mods/ModManager.hpp>

using namespace phx::mods;

static int CustomExceptionHandler(
    lua_State* l, sol::optional<const std::exception&> maybe_exception,
    sol::string_view description)
{
	std::string error;
	if (maybe_exception)
	{
		const std::exception& ex = *maybe_exception;
		error                    = ex.what();
	}
	else
	{
		error = description;
	}

	LOG_FATAL("MODDING") << "An unexpected error has occured. \n"
	                     << "\t>> " << error;

	// you must push 1 element onto the stack to be
	// transported through as the error object in Lua
	// note that Lua -- and 99.5% of all Lua users and libraries -- expects a
	// string so we push a single string (in our case, the description of the
	// error)
	return sol::stack::push(l, description);
}

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

	LOG_FATAL("MODDING") << "An unexpected Lua error has occured. " << error << "The application will now be aborted.";
}

ModManager::ModManager(const ModList& modList) : m_modsRequired(modList)
{
	m_luaState.open_libraries(sol::lib::base);
	m_luaState.set_exception_handler(&CustomExceptionHandler);
	m_luaState.set_panic(sol::c_call<decltype(&CustomPanicHandler), &CustomPanicHandler>);

	sol::protected_function_result pfr =
	    m_luaState.safe_script("wassup fam", &sol::script_pass_on_error);
	sol::error err = pfr;
	LOG_FATAL("MODDING") << err.what();
}

ModManager::Status ModManager::load(float* progress) { return {true}; }

void ModManager::cleanup() {}

const ModManager::ModList& ModManager::getModList() const
{
	return m_modsRequired;
}

const Privileges* ModManager::getPrivileges() const { return &m_privileges; }

const CommandBook* ModManager::getCommandBook() const { return &m_commandBook; }
