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

#include <functional>
#include <string>
#include <vector>

namespace phx::mods
{
	class Privileges
	{
	public:
		Privileges();
		~Privileges();

		void registerPrivilege(const std::string& priv);

		// privList is a comma-delimited list.
		bool hasPrivilege(const std::string& privList,
		                  const std::string  priv) const;

	private:
		std::vector<std::string> m_privileges;
	};

	class CommandBook
	{
	public:
		using CommandFunction = std::function<void(std::vector<std::string>)>;

		struct Command
		{
			std::string     help;
			std::string     privs;
			CommandFunction func;
		};

	public:
		CommandBook();
		~CommandBook();

		// will allow for a command to work from multiple different privileges.
		void registerCommand(const std::string&     command,
		                     const std::string&     help,
		                     const std::string&     privilege,
		                     const CommandFunction& func);

		bool           commandExists(const std::string& command);
		const Command* getCommand(const std::string& command);

	private:
		// key is the actual command.
		std::unordered_map<std::string, Command> m_commands;
	};
} // namespace phx::mods
