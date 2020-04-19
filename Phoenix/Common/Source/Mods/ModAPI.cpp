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
#include <Common/Mods/ModAPI.hpp>

#include <algorithm>
#include <sstream>

using namespace phx::mods;

Privileges::Privileges()  = default;
Privileges::~Privileges() = default;

void Privileges::registerPrivilege(const std::string& priv)
{
	auto it = std::find(m_privileges.begin(), m_privileges.end(), priv);
	if (it == m_privileges.end())
	{
		m_privileges.push_back(priv);
	}
}

bool Privileges::hasPrivilege(const std::string& privList,
                              const std::string  priv) const
{
	if (priv.length() == 0)
	{
		return true;
	}

	// split comma delimited list into array.
	std::vector<std::string> privs;
	std::stringstream        sstream(privList);
	std::string              substr;
	while (std::getline(sstream, substr, ','))
	{
		privs.push_back(substr);
	}

	// search array for privilege.
	auto it = std::find(m_privileges.begin(), m_privileges.end(), priv);
	if (it == m_privileges.end())
	{
		return false;
	}

	return true;
}

CommandBook::CommandBook()
{
	m_commands["help"] = {"Type /help [command] to learn more about a command.",
	                      "", [=](std::vector<std::string> args) {
		                      if (args.empty())
		                      {
			                      return m_commands["help"].help;
		                      }

		                      return m_commands[args[0]].help;
	                      }};

	m_commands["list"] = {
	    "Type /list to list every command, use a privilege as a parameter to "
	    "see what commands are available to that role.",
	    "", [=](std::vector<std::string> args) {
		    if (args.empty())
		    {
			    std::string commands;
			    for (auto& command : m_commands)
			    {
				    commands += command.first + "\n";
			    }

			    return commands;
		    }
	    }};
}

void CommandBook::registerCommand(const std::string&     command,
                                  const std::string&     help,
                                  const std::string&     privilege,
                                  const CommandFunction& func)
{
	auto it = m_commands.find(command);
	if (it == m_commands.end())
	{
		LOG_DEBUG("[MODDING]")
		    << "The command \" " << command
		    << "\" has already been defined by another mod, the formally "
		       "registered command shall be used.";
	}
	else
	{
		m_commands[command] = {help, privilege, func};
	}
}

bool CommandBook::commandExists(const std::string& command) const
{
	return m_commands.find(command) != m_commands.end();
}

const CommandBook::Command* CommandBook::getCommand(const std::string& command) const
{
	
}
