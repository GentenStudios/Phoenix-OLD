// Copyright 2020 Genten Studios
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

#include <Common/CLIParser.hpp>
#include <Common/Logger.hpp>

#include <cstring>
#include <sstream>
#include <algorithm>

using namespace phx;

void CLIParser::addParameter(const CLIParameter& parameter)
{
	const auto it = m_fullParamMap.find(parameter.parameter);
	if (it == m_fullParamMap.end())
	{
		if (parameter.enableShorthand)
		{
			const auto shortHandIt = m_shorthandMap.find(parameter.shorthand);
			if (shortHandIt == m_shorthandMap.end())
			{
				m_parameters.push_back(parameter);
				m_fullParamMap.insert(
				    {parameter.parameter, m_parameters.size()});
				m_shorthandMap.insert(
				    {parameter.shorthand, m_parameters.size()});
			}
			else
			{
				LOG_WARNING("CLI")
				    << "The shorthand parameter: " << parameter.shorthand
				    << " has already been used by another parameter. Either "
				       "disable the shorthand, or select another character.";
			}
		}
		else
		{
			m_parameters.push_back(parameter);
			m_fullParamMap.insert({parameter.parameter, m_parameters.size()});
		}
	}
	else
	{
		LOG_WARNING("CLI") << "The parameter: " << parameter.parameter
		                   << " has already been selected, either disable this "
		                      "parameter or select another name";
	}
}

bool CLIParser::parse(int argc, char** argv)
{
	constexpr const char* DoubleStroke = "--"; // for full parameters.
	constexpr const char* SingleStroke = "-";  // for shorthand.

	// we start at 1 because argument 0 is the command used to run the program.
	int index = 1;
	while (index < argc)
	{
		const std::size_t argLen = strlen(argv[index]);
		if (argLen < 2)
		{
			// if it's less than two characters, then it's not enough for smth
			// lik -c, and it's not enough --command (obviously)

			LOG_FATAL("CLI") << "Invalid Command Line arguments.";
			LOG_FATAL("CLI") << getHelpString();

			return false;
		}

		// two because two characters in the DoubleStroke.
		if (strncmp(DoubleStroke, argv[index], 2) == 0)
		{
			std::string key {argv[index] + 2, argv[index] + argLen};

			const auto it =
			    std::find_if(m_parameters.begin(), m_parameters.end(),
			                 [key](const CLIParameter& param) {
				                 if (key == param.parameter)
				                 {
					                 return true;
				                 }

				                 return false;
			                 });

			if (it == m_parameters.end())
			{
				// not found.

				LOG_FATAL("CLI") << "Unrecognized Parameter: " << key;
				LOG_FATAL("CLI") << "Refer to the help below: ";
				LOG_FATAL("CLI") << getHelpString();

				return false;
			}

			// now actually handle what comes next.

			// if the arg is a flag, keep going.
			if ((*it).isFlag)
			{
				// we don't need to do anything now, increment and dip.
				++index;
				continue;
			}

			// we now know it's not a flag, so lets make sure there's an
			// argument following this to work with.
			if (!(index + 1 < argc))
			{
				// there isn't, so lets say there's an error.

				LOG_FATAL("CLI") << "Expected a value for this parameter ("
				                 << key << "). None were given.";
				LOG_FATAL("CLI") << "Refer to the help below: ";
				LOG_FATAL("CLI") << getHelpString();

				return false;
			}

			if ((*it).multiValue)
			{
				while (index < argc)
				{
					// every argument till the next - or -- is an argument for
					// this.

					if (strncmp(DoubleStroke, argv[index], 2) == 0)
					{
						break;
					}

					if (strncmp(SingleStroke, argv[index], 1) == 0)
					{
						break;
					}

					m_arguments[m_fullParamMap.at(key)].push_back(argv[index]);

					++index;
				}
			}
			else
			{
				m_arguments[m_fullParamMap.at(key)] = {argv[index + 1]};

				// we do a += 2 here because we want to move to the next
				// parameter, but the next *argument* is NOT a parameter it's
				// the value associated with the current parameter.
				//
				// --command Argument --param2
				// ^                  ^
				// index = 1          index = 3
				index += 2;
			}
		}
		else if (strncmp(SingleStroke, argv[index], 1) == 0)
		{
			std::string key {argv[index] + 1, argv[index] + argLen};

			const auto it =
			    std::find_if(m_parameters.begin(), m_parameters.end(),
			                 [key](const CLIParameter& param) {
				                 if (key == param.shorthand)
				                 {
					                 return true;
				                 }

				                 return false;
			                 });

			if (it == m_parameters.end())
			{
				// not found.

				LOG_FATAL("CLI") << "Unrecognized Shorthand Parameter: " << key;
				LOG_FATAL("CLI") << "Refer to the help below: ";
				LOG_FATAL("CLI") << getHelpString();

				return false;
			}

			// if the arg is a flag, keep going.
			if ((*it).isFlag)
			{
				// we don't need to do anything now, increment and dip.
				++index;
				continue;
			}

			// we now know it's not a flag, so lets make sure there's an
			// argument following this to work with.
			if (!(index + 1 < argc))
			{
				// there isn't, so lets say there's an error.

				LOG_FATAL("CLI")
				    << "Expected a value for this shorthand parameter (" << key
				    << "). None were given.";
				LOG_FATAL("CLI") << "Refer to the help below: ";
				LOG_FATAL("CLI") << getHelpString();

				return false;
			}

			if ((*it).multiValue)
			{
				while (index < argc)
				{
					// every argument till the next - or -- is an argument for
					// this.

					if (strncmp(DoubleStroke, argv[index], 2) == 0)
					{
						break;
					}

					if (strncmp(SingleStroke, argv[index], 1) == 0)
					{
						break;
					}

					m_arguments[m_shorthandMap.at(key)].push_back(argv[index]);

					++index;
				}
			}
			else
			{
				m_arguments[m_shorthandMap.at(key)] = {argv[index + 1]};

				// we do a += 2 here because we want to move to the next
				// parameter, but the next *argument* is NOT a parameter it's
				// the value associated with the current parameter.
				//
				// --command Argument --param2
				// ^                  ^
				// index = 1          index = 3
				index += 2;
			}
		}
	}

	return true;
}

std::string CLIParser::getHelpString() const
{
	std::stringstream help;

	for (auto& param : m_parameters)
	{
		help << "\n";
		help << "--" << param.parameter;
		help << " ("
		     << "-" << param.shorthand << "): ";
		help << param.helpString;
	}

	return help.str();
}

std::vector<std::string> const* CLIParser::getArgument(
    const std::string& parameter) const
{
	const auto it = m_fullParamMap.find(parameter);
	if (it == m_fullParamMap.end())
	{
		// parameter not found.
		return nullptr;
	}

	const auto it2 = m_arguments.find((*it).second);
	if (it2 == m_arguments.end())
	{
		// argument not found from parsed list.
		return nullptr;
	}

	return &((*it2).second);
}
