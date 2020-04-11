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

#include <Common/InputMap.hpp>

#include <Common/ContentLoader.hpp>

#include <SDL.h>

using namespace phx::client;
using namespace phx;

InputMap::InputMap()
{
	ContentManager::get()->lua["core"]["input"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * ---
	     * @subsection coreinput core.input
	     * @brief Interfaces with the player
	     */
	    ContentManager::get()->lua.create_table();
	ContentManager::get()->lua["core"]["input"]["registerInput"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreplayerregisterinput
	     * core.player.registerInput(string uniqueName, string displayName,
	     * string defaultKey)
	     * @brief Registers a new input
	     * @param uniqueName the unique name for the input in the format
	     * "modname:inputname"
	     * @param displayName the display name that is shown to the user when
	     * binding keys
	     * @param defaultKey the default key that is first registered if the
	     * user does not set one
	     * @warning The default key is currently not functional, it will be set
	     * to 0 every time
	     *
	     * @return The input index (*Not functional at this time)
	     *
	     * @todo convert from string provided by lua to enum
	     */
	    [this](std::string uniqueName, std::string displayName,
	           std::string defaultKey) {
		    registerInput(uniqueName, displayName, events::Keys::KEY_0);
		    return m_currentInputRef;
	    };
	ContentManager::get()->lua["core"]["input"]["getInput"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreplayergetInput core.player.getInput(int input)
	     * @brief Gets the state of the input
	     * @param input The reference of the input you are looking for
	     *
	     * @return The state of the input
	     */
	    [this](int input) { return getState(input); };
	ContentManager::get()->lua["core"]["input"]["getInputRef"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreplayergetInputRef core.player.getInputRef(string
	     * uniqueName)
	     * @brief Gets the state of the input
	     * @param uniqueName the unique name used when registering the input
	     *
	     * @return The reference of the input
	     */
	    [this](std::string uniqueName) { return getReference(uniqueName); };
	ContentManager::get()->lua["core"]["input"]["registerCallback"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreplayerregisterCallback
	     * core.player.getRegisterCallback(int input, function f)
	     * @brief Registers a callback to be called when an input event happens
	     * @param input The reference of the input you are looking for
	     * @param f The function that is called
	     */
	    [this](int input, sol::function f) { attachCallbackToInput(input, f); };
}

InputMap::~InputMap() = default;

void InputMap::initialize()
{
	m_uniqueInputs["core:none"] = m_currentInputRef;
	m_inputs[m_currentInputRef] = {"None", "core:none"};
	++m_currentInputRef;
}

void InputMap::onEvent(events::Event e)
{
	if (e.type == events::EventType::KEY_PRESSED)
	{
		auto it = m_inputs.begin();
		while (it != m_inputs.end())
		{
			if (it->second.key == e.keyboard.key)
			{
				// calls the std::function. we don't need to check if it's
				// "nullptr" (std::function will compare to nullptr if empty.)
				// because it literally cannot get there if it's empty.
				m_callbacks[it->first]();
				break;
			}

			++it;
		}
	}
}

Input* InputMap::registerInput(const std::string& uniqueName,
                               const std::string& displayName,
                               events::Keys       defaultKey)
{
	m_uniqueInputs[uniqueName] = m_currentInputRef;
	m_inputs[m_currentInputRef] = {displayName, uniqueName, defaultKey, defaultKey};
	++m_currentInputRef;
	return &m_inputs[m_currentInputRef - 1];
}

void InputMap::attachCallbackToInput(const std::string&           uniqueName,
                                     const std::function<void()>& func)
{
	m_callbacks[m_uniqueInputs.at(uniqueName)] = func;
}

void InputMap::attachCallbackToInput(InputRef                     primaryKey,
                                     const std::function<void()>& func)
{
	m_callbacks[primaryKey] = func;
}

Input* InputMap::getInput(const std::string& uniqueName)
{
	return &m_inputs[m_uniqueInputs.at(uniqueName)];
}

Input* InputMap::getInput(InputRef primaryKey)
{
	return &m_inputs.at(primaryKey);
}

void InputMap::setInput(const std::string& uniqueName, events::Keys key)
{
	getInput(uniqueName)->key = key;
}

void InputMap::setInput(InputRef primaryKey, events::Keys key)
{
	getInput(primaryKey)->key = key;
}

bool InputMap::getState(const std::string& uniqueName)
{
	return SDL_GetKeyboardState(
	    nullptr)[static_cast<SDL_Scancode>(getInput(uniqueName)->key)];
}

bool InputMap::getState(InputRef primaryKey)
{
	return SDL_GetKeyboardState(
	    nullptr)[static_cast<SDL_Scancode>(getInput(primaryKey)->key)];
}

bool InputMap::getState(Input* input)
{
	return SDL_GetKeyboardState(
	    nullptr)[static_cast<SDL_Scancode>(input->key)];
}

InputMap::InputRef InputMap::getReference(const std::string& uniqueName)
{
	for (const auto& input : m_uniqueInputs)
	{
		if (input.first == uniqueName)
		{
			return input.second;
		}
	}
	return -1;
}