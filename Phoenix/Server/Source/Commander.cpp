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

#include <Common/Commander.hpp>
#include <Common/ContentLoader.hpp>

using namespace phx;

int CommandBook::find(const std::string& command)
{
	for (int j = 0; j < m_page; j++)
	{
		if (m_command[j] == command)
		{
			return j;
		}
	}
	return -1;
}

void CommandBook::add(const std::string& command, const std::string& help,
                      const std::string& permission, const CommandFunction& f)
{
	int j = find(command);
	// If command does not already exist, enter new command
	if (j == -1)
	{
		j = m_page;
		++m_page;
	}
	m_command.push_back(command);
	m_help.push_back(help);
	m_permission.push_back(permission);
	m_functions.push_back(f);
}

int CommandBook::getPage() { return m_page; }

CommandBook::CommandBook()
{
	ContentManager::get()->lua["core"]["command"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * ---
	     * @subsection corecmd core.command
	     * @brief Interfaces with the commander
	     *
	     */
	    ContentManager::get()->lua.create_table();
	ContentManager::get()->lua["core"]["command"]["register"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection corecmdreg core.command.register
	     * @brief Registers a new command
	     *
	     * In the terminal typing "/" followed by a command will execute the
	     *command
	     *
	     * @param command The command to register
	     * @param help A helpstring that is printed to terminal when typing
	     *"/help <command>"
	     * @param f The callback function that is called by the commander
	     * The callback function must take a table as an argument
	     * Any arguments included when the command is executed will be passed in
	     *this table
	     *
	     * @b Example:
	     * @code {.lua}
	     * function hello (args)
	     *     if args[1] == "there" then
	     *         print("General Kenobi")
	     *	   elseif args[1] == "world" then
	     * 		   print("World says hi")
	     *	   else
	     *         print("with you, the force is not")
	     *     end
	     * end
	     * core.command.register("Hello", "Master the arts of the Jedi you
	     *must", hello)
	     * @endcode
	     */
	    [](std::string command, std::string help, sol::function f) {
		    CommandBook::get()->add(command, help, "all", f);
	    };
}

Commander::Commander() : m_book(CommandBook::get()) {}

bool Commander::help(const std::vector<std::string>& args, std::ostream& out)
{
	if (args.empty())
	{
		out << "Type /help [command] to learn more about a command \nType "
		       "/list for a list of available commands\n";
		return true;
	}
	else if (args[0] == "help")
	{
		out << "Type /help [command] to learn more about a command \n";
		return true;
	}
	else if (args[0] == "list")
	{
		out << "Lists available commands\n";
		return true;
	}
	const int j = m_book->find(args[0]);
	if (j == -1)
	{
		out << "Command \"" + args[0] + "\" not found \n";
		return false;
	}
	else
	{
		out << m_book->m_help[j];
		return true;
	}
}

bool Commander::run(const std::string&              command,
                    const std::vector<std::string>& args, std::ostream& out)
{
	// Check for built in functions
	if (command == "help")
	{
		return this->help(args, out);
	}
	else if (command == "list")
	{
		this->list(out);
		return true;
	}
	// If no built in functions match, search library
	const int j = m_book->find(command);
	if (j == -1)
	{
		out << "Command \"" + command + "\" not found \n";
		return false;
	}
	else
	{
		m_book->m_functions[j](args);
		return true;
	}
}

void Commander::list(std::ostream& out)
{
	out << "Available commands\n";
	for (int j = 0; j < m_book->getPage(); j++)
	{
		out << "-" + m_book->m_command[j] + "\n";
	}
}

void Commander::post(std::istream& in, std::ostream& out)
{
	std::string input;
	while (true)
	{
		out << "\n->";
		int                      i = 0;
		std::vector<std::string> args;
		std::string              command = "";
		std::string              buffer;
		in >> command;
		while (in.peek() != '\n')
		{
			in >> buffer;
			args.push_back(buffer);
			i++;
		}
		if (command == "exit")
		{
			break;
		}
		run(command, args, out);
	}
}

void Commander::callback(const std::string& input, std::ostringstream& cout)
{
	cout << "->" << input << "\n";

	// String views are cheaper and since the push_back vector function
	// copies the contents of the input string as well we can avoid
	// directly copying characters for the most part here.
	std::string_view search = input;
	std::string      command;
	std::string arg; // Just used to copy the args out of the search string.
	std::vector<std::string> args;
	size_t                   searchLoc;
	size_t                   spaceLoc;

	// Substring was unnecessary because it creates a duplicate string
	// to store the memory in when we can just refference it statically.
	if (!search.empty() && search[0] == '/')

	{
		// NOTE:
		//   Can't enter \t or \n rn, might be a good idea to sanitize l8r
		// Search `first_of` initially, it's faster when there's no spaces.
		spaceLoc = search.find_first_of(' ');

		// if we don't have arguments don't try and populate the args array.
		if (spaceLoc != std::string_view::npos)
		{
			command = search.substr(1, spaceLoc - 1);

			// doesn't create a new string object, just moves the start forward.
			// negative offset handled by leading char
			search.remove_prefix(spaceLoc);

			// `first_not_of` space keeps errors from happening if a user
			// accidentally separates the args with extra whitespace.
			while ((searchLoc = search.find_first_not_of(' ')) !=
			       std::string_view::npos)
			{
				search.remove_prefix(searchLoc); // strip the leading whitspace
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
		run(command, args, cout);
	}
}

