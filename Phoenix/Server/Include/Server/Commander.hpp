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
 * @file Commander.hpp
 * @brief Header file to implement a command execution system designed to
 * interface with a terminal.
 *
 * @copyright Copyright (c) 2019-2020
 *
 */

#pragma once

#include <Server/Iris.hpp>

#include <entt/entity/registry.hpp>

#include <functional>
#include <sstream>
#include <string>
#include <vector>

namespace phx::server
{
	/**
	 * @brief A function that is called when a command is executed.
	 *
	 * The function should take a vector of strings. Each string is an argument
	 * similar to how programs are called from the terminal.
	 *
	 */
	typedef std::function<void(std::vector<std::string> args)> CommandFunction;

	struct Command{
	    std::string command;
	    std::string help;
	    CommandFunction callback;
	};

	/**
	 * @brief The command book stores commands and information on them to be
	 * used by a commander.
	 */
	class Commander
	{
	public:
		Commander(entt::registry* registry, networking::Iris* iris);

		/**
		 * @brief Registers a command in the command registry.
		 *
		 * If a command already exists in the registry, this function will
		 * over write that command with the new data.
		 *
		 * @param command The keyword for calling the command.
		 * @param help A help string that can be displayed to the user.
		 * @param f The function that is called when the command is executed.
		 */
		static void add(const std::string& command, const std::string& help,
		                const CommandFunction& f);

		/**
		 * @brief Calls a command.
		 *
		 * @param command The keyword for calling the command.
		 * @param args The arguments to be passed to the command.
		 * @param out An output stream to output any return from internal.
		 * commander functions
		 * @return Returns True if the function was called and False if the
		 * function could not be found
		 */
		static bool run(entt::entity* userRef, const std::string& input);

		/**
		 * @brief Returns helpstring for command.
		 *
		 * @param args array of input, args[0] is the command helpstring is
		 * returned for, all other array values are not used.
		 * @param out An output stream to output any help text to.
		 * @return Returns True if successful and False if it could not find
		 * the inputted command.
		 */
		static bool help(entt::entity*                   userRef,
		                 const std::vector<std::string>& args);

		/**
		 * @brief Outputs a string listing available commands.
		 *
		 * @param out The output stream the list of commands is sent to.
		 */
		static void list(entt::entity* userRef);

	private:
		static entt::registry*   m_registry;
		static networking::Iris* m_iris;
	};
} // namespace phx
