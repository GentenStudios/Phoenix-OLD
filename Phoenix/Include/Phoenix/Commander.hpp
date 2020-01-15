// Copyright 2019 Genten Studios
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
 * @brief Header file to implement a command execution system designed to interface with a terminal.
 * 
 * @copyright Copyright (c) 2019-2020
 * 
 */

#pragma once

#include <Phoenix/Singleton.hpp>

#include <functional>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace phx
{
    /**
     * @brief A function that is called when a command is executed
     * 
     * The function should take a vector of strings. Each string is an argument similar to how programs are called from the terminal.
     * 
     */
    typedef std::function<void(std::vector<std::string> args)> commandFunction;

    /**
     * @brief The command book stores commands and information on them to be
     * used by a commander
     */

    struct CommandBook : public Singleton<CommandBook>
    {
        std::vector<std::string> m_command;
        std::vector<std::string> m_help;
        std::vector<std::string> m_permission;
        std::vector<commandFunction>    m_functions;

        /**
         * @brief Registers a command in the command registry
         *
         * @param command The keyword for calling the command
         * @param permission What permission is required to run this command
         */
        void add(const std::string& command, const std::string& help,
                    const std::string& permission, commandFunction f);

        /**
         * @brief Searches for a command
         *
         * @param command The command to search for
         * @return index command is located at or -1 if its not found
         */
        int find(const std::string& command);

        /**
         * @brief Gets next open space in book
         *
         * @return Returns the next open space in arrays as an integer
         */
        int getPage();

    private:
        /**
         * @brief Counter that keep track of next space in the arrays (eg
         * next page in the book)
         */
        int m_page;
    };

    /**
     * @brief The Commander handles comand line functions. It accepts an in
     * and out stream on creation and can interact with the command line or
     * another interface.
     *
     */
    class Commander
    {
    private:
        CommandBook*  m_book;

    public:
        /**
         * @brief Initializes a commander that can run and execute commands
         */
        Commander();
        ~Commander();

        /**
         * @brief Calls a command
         *
         * @param command The keyword for calling the command
         * @param args The arguments to be passed to the command
         * @param out An output stream to output any return from internal commander functions
         * @return Returns True if the function was called and False if the
         * function could not be found
         */
        bool run(const std::string&               command,
                 const std::vector<std::string>&& args,
                 std::ostream&                    out);

        /**
         * @brief Returns helpstring for command
         *
         * @param args array of input, args[0] is the command helpstring is
         * returned for, all other array values are not used
         * @param out An output stream to putput any help text to
         * @return Returns True if successful and False if it could not find
         * the innputted command
         */
        bool help(const std::vector<std::string>&& args,
                         std::ostream&             out);

        /**
         * @brief Outputs a string listing available commands
         * 
         * @param out The output stream the list of commands is sent to
         */
        void list(std::ostream& out);

        /**
         * @brief Terminal interface to listen for and execute commands
         * 
         * @param in An input stream to get input, usually (but not nessecarily) std::cin
         * @param out An output stream output is sent to, usually (but not nessecarily) std::cout
         */
        void post(std::istream& in, std::ostream& out);

        /**
         * @brief A callback function that runs a single command and outputs to a stream
         * 
         * @param input Input string containing the command and arguments to be ran
         * @param out The output stream any output goes to
         */
        void callback(const std::string &input, std::ostringstream &cout);
    };
} // namespace qz