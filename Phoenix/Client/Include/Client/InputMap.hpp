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
 * I kinda roughed this one, @beeper I will leave it up to your best judgement.
 * Either we can try to leave an Input as a pure struct to store data, or we can
 * turn it into a full class nearly mimicking the settings system. There will be
 * a lot of learning as we build this so no rush on making it perfect. I didn't
 * put a ton of effort making sure this matched the event system, it also would
 * likely do better inside the Events space itself? Maybe?
 *
 * This system is purely a smart data storage, it itself should have no clue
 * what an event, callback, or state is. Its simply an abstraction on top of the
 * mouse and keyboard state and is only used to translate between string or
 * numeral identifiers for an input, and the SDL key it is mapped to.
 *
 * The string identifier "uniqueKey" should only be used when interacting
 * outside of the current execution (where memory is not preserved such as in
 * the save or when hard-coding an input), when working during run-time, we
 * should only use the primaryKey to reference an input to reduce lookup times.
 */

#pragma once

#include <Client/Events/Event.hpp>

#include <string>

namespace phx::client
{
	struct Input
	{
		/// @brief The Human readable name displayed in menus
		std::string displayName;

		/// @brief The unique key used to identify input in save file and mods.
		/// In the format core::moveforward
		std::string uniqueKey;

		/// @brief The physical kep assigned to this input
		events::Event key;

		/// @brief The default key assigned to this input
		events::Event default;
	};

	/// @todo @beeper If we handle lua callback registration during
	/// initialization, then this does not need to be a singleton as there are
	/// only two systems that need to see it. Those can get this system passed
	/// as an argument instead.

	class InputMap
	{
	public:
		/**
		 * @brief Initializes the InputMapper and loads previously saved data
		 * from a file.
		 */
		InputMap();
		~InputMap() = default;

		/**
		 * @brief Registers an input in the input system.
		 *
		 * This checks for a matching input in m_unused to grab a pre-set input
		 * assignment.
		 *
		 * This should only be used by the event system. The event
		 * system will register an input when a new event is registered and use
		 * its primaryKey when polling events to get the associated key.
		 *
		 * @param uniqueKey The unique key for the input in the format
		 * core:moveforward
		 * @param displayName The Human readable name displayed in menus
		 * @param key The physical kep assigned to this input
		 * @return The primaryKey location of this input for use by the system
		 * that registered the input
		 */
		std::size_t registerInputEvent(std::string uniqueKey,
		                          std::string displayName, events::Event key);

		/// Or should we just return the actual SDL key?
		InputMap* getInput(std::string uniqueKey);
		InputMap* getInput(std::size_t primaryKey);
		
		///@brief This gets a full list of all inputs for use in the settings
		///menu
		auto getInputs();

		void setInput(std::string uniqueKey, events::Event key);
		void setInput(std::size_t primaryKey, events::Event key);

	private:
		/**
		 * @brief Load function loads all inputs from input.config
		 */
		///@todo @beeper, this should only be called during the construction of
		///the InputMapper. This should take some hints from the settings system
		///and also load a buffer of unregistered inputs
		void load();
		/**
		 * @brief Save function saves all non-default inputs to input.config
		 */
		///@todo @beeper, this should be called every time an input is edited
		void save();

		std::vector<InputMap> m_inputs;
		std::vector<InputMap> m_unused;
	};
} // namespace phx::client
