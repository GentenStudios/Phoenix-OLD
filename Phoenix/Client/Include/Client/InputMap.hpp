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

#include <Client/Events/Event.hpp>
#include <Client/Events/IEventListener.hpp>

#include <Common/Singleton.hpp>

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace phx::client
{
	struct Input
	{
		/// @brief The Human readable name displayed in menus
		std::string displayName;

		/// @brief The unique key used to identify input in save file and mods.
		/// In the format core::moveforward
		std::string uniqueKey;

		/// @brief The physical key assigned to this input
		events::Keys key = events::Keys::NONE;

		/// @brief The default key assigned to this input
		events::Keys defaultKey = events::Keys::NONE;
	};

	class InputMap : public Singleton<InputMap>, public events::IEventListener
	{
	public:
		using InputRef = int;

		InputMap();
		~InputMap();

		void initialize();

		void onEvent(events::Event e) override;

		Input* registerInput(const std::string& uniqueName,
		                     const std::string& displayName,
		                     events::Keys       defaultKey);

		void attachCallbackToInput(const std::string&    uniqueName,
		                           const std::function<void()>& func);
		void attachCallbackToInput(InputRef              primaryKey,
		                           const std::function<void()>& func);

		// if input does not exist, will reply with key unknown.
		Input* getInput(const std::string& uniqueName);
		Input* getInput(InputRef primaryKey);

		// no need for a Input* overload since it's a pointer so you can just
		// change the key directly.
		void setInput(const std::string& uniqueName, events::Keys key);
		void setInput(InputRef primaryKey, events::Keys key);

		// try to not use this, inefficient.
		bool getState(const std::string& uniqueName);

		bool getState(InputRef primaryKey);
		bool getState(Input* input);

		InputRef getReference(const std::string& uniqueName);

	private:
		unsigned int                              m_currentInputRef = 0;
		std::unordered_map<std::string, InputRef> m_uniqueInputs;
		std::unordered_map<InputRef, Input>       m_inputs;

		// remember, input ref is just an int.
		std::unordered_map<InputRef, std::function<void()>> m_callbacks;
	};
} // namespace phx::client
