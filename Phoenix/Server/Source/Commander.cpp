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

/**
 * @file Commander.cpp
 * @brief Source file to implement a command execution system designed to
 * interface with a terminal.
 *
 * @copyright Copyright (c) 2019-2020
 *
 */

#include <Server/Commander.hpp>
//#include <Common/ContentLoader.hpp>

using namespace phx::server;

Commander::Commander(entt::registry* registry, networking::Iris* iris)
{
	m_iris     = iris;
	m_registry = registry;
	//    ContentManager::get()->lua["core"]["command"] =
	//        /**
	//         * @addtogroup luaapi
	//         *
	//         * ---
	//         * @subsection corecmd core.command
	//         * @brief Interfaces with the commander
	//         *
	//         */
	//        ContentManager::get()->lua.create_table();
	//    ContentManager::get()->lua["core"]["command"]["register"] =
	//        /**
	//         * @addtogroup luaapi
	//         *
	//         * @subsubsection corecmdreg core.command.register
	//         * @brief Registers a new command
	//         *
	//         * In the terminal typing "/" followed by a command will execute
	//         the *command
	//         *
	//         * @param command The command to register
	//         * @param help A helpstring that is printed to terminal when
	//         typing
	//         *"/help <command>"
	//         * @param f The callback function that is called by the commander
	//         * The callback function must take a table as an argument
	//         * Any arguments included when the command is executed will be
	//         passed in *this table
	//         *
	//         * @b Example:
	//         * @code {.lua}
	//         * function hello (args)
	//         *     if args[1] == "there" then
	//         *         print("General Kenobi")
	//         *	   elseif args[1] == "world" then
	//         * 		   print("World says hi")
	//         *	   else
	//         *         print("with you, the force is not")
	//         *     end
	//         * end
	//         * core.command.register("Hello", "Master the arts of the Jedi you
	//         *must", hello)
	//         * @endcode
	//         */
	//        [](std::string command, std::string help, sol::function f) {
	//          Commander.add(command, help, f);
	//        };
}

void Commander::add(const std::string& command, const std::string& help,
                    const CommandFunction& f)
{
	auto view = m_registry->view<Command>();
	for (auto entity : view)
	{
		auto& com = view.get<Command>(entity);
		if (com.command == command)
		{
			com.help     = help;
			com.callback = f;
			return;
		}
	}
	auto entity = m_registry->create();
	m_registry->emplace<Command>(entity, command, help, f);
}

bool Commander::run(entt::entity* userRef, const std::string& input)
{
	// Break the command into args
	std::string_view search = input;
	std::string      command;
	std::string arg; // Just used to copy the args out of the search string.
	std::vector<std::string> args;
	size_t                   searchLoc;
	size_t                   spaceLoc;

	if (!search.empty() && search[0] == '/')
	{
		// @TODO Can't enter \t or \n rn, might be a good idea to sanitize later
		// Search `first_of` initially, it's faster when there's no spaces.
		spaceLoc = search.find_first_of(' ');

		// if we don't have arguments don't try and populate the args array.
		if (spaceLoc != std::string_view::npos)
		{
			command = search.substr(1, spaceLoc - 1);

			search.remove_prefix(spaceLoc);

			while ((searchLoc = search.find_first_not_of(' ')) !=
			       std::string_view::npos)
			{
				search.remove_prefix(searchLoc); // strip the leading whitespace
				spaceLoc = search.find_first_of(' ');

				if (spaceLoc != std::string_view::npos)
				{
					arg =
					    search.substr(0, spaceLoc); // gen new string from view
					args.push_back(arg);
				}

				// Don't forget to double check if spaceLoc is npos before we
				// remove_prefix, otherwise we might end up out of bounds...
				else
				{
					arg = search.substr(0, search.length());
					args.push_back(arg);
					break;
				}

				search.remove_prefix(spaceLoc);
			}
		}
		else
		{
			// otherwise just use the whole string without the command char.
			command = search.substr(1, search.length());
		}
	}

	// Check for built in functions
	if (command == "help")
	{
		return help(userRef, args);
	}
	else if (command == "list")
	{
		list(userRef);
		return true;
	}

    // If no built in functions match, search library
	auto view = m_registry->view<Command>();
	for (auto entity : view)
	{
		auto& com = view.get<Command>(entity);
		if (com.command == command)
		{
			com.callback(args);
			return true;
		}
	}
	// No commands match
	return false;
}

bool Commander::help(entt::entity*                   userRef,
                     const std::vector<std::string>& args)
{
	if (args.empty())
	{
		m_iris->sendMessage(
		    userRef, "Type /help [command] to learn more about a command "
		             "\nType /list for a list of available commands\n");
		return true;
	}
	else if (args[0] == "help")
	{
		m_iris->sendMessage(
		    userRef, "Type /help [command] to learn more about a command \n");
		return true;
	}
	else if (args[0] == "list")
	{
		m_iris->sendMessage(userRef, "Lists available commands\n");
		return true;
	}

	auto view = m_registry->view<Command>();
	for (auto entity : view)
	{
		auto& com = view.get<Command>(entity);
		if (com.command == args[0])
		{
			m_iris->sendMessage(userRef, com.help + "\n");
			return true;
		}
	}
	m_iris->sendMessage(userRef, "Command \"" + args[0] + "\" not found \n");
	return false;
}

void Commander::list(entt::entity* userRef)
{
	m_iris->sendMessage(userRef, "Available commands:\n");
	auto view = m_registry->view<Command>();
	for (auto entity : view)
	{
		auto& com = view.get<Command>(entity);
		m_iris->sendMessage(userRef, "-" + com.command + "\n");
	}
}