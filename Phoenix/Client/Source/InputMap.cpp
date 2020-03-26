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

#include <Client/InputMap.hpp>

#include <SDL.h>

using namespace phx::client;
using namespace phx;

InputMap::InputMap()
{
	// austin write lua stuff here.
}

InputMap::~InputMap() = default;

void InputMap::initialize()
{
	// this just exists to make sure someone calls the initialize function.
}

void InputMap::onEvent(events::Event e)
{
	
}

InputMap::InputRef InputMap::registerInput(const std::string& uniqueName,
                                           const std::string& displayName,
                                           events::Keys       defaultKey)
{
}

void InputMap::attachCallbackToInput(const std::string&    uniqueName,
                                     std::function<void()> func)
{
}

void InputMap::attachCallbackToInput(InputRef              primaryKey,
                                     std::function<void()> func)
{
}

Input* InputMap::getInput(const std::string& uniqueName) {}

Input* InputMap::getInput(InputRef primaryKey) {}

void InputMap::setInput(const std::string& uniqueName, events::Keys key) {}

void InputMap::setInput(InputRef primaryKey, events::Keys key) {}

bool InputMap::getState(const std::string& uniqueName) {}

bool InputMap::getState(InputRef primaryKey) {}

bool InputMap::getState(Input* input) {}

void InputMap::load() {}

void InputMap::save() {}
