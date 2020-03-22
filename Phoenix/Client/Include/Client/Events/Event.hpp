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

#include <Client/Events/Keys.hpp>

#include <SDL.h>

#include <cstring>
#include <functional>
#include <string>

namespace phx::events
{
	struct Position
	{
		int x = {};
		int y = {};
	};

	struct Size
	{
		int width  = {};
		int height = {};
	};

	struct Scroll
	{
		double x = {};
		double y = {};
	};

	struct Keyboard
	{
		Keys key  = {};
		Mods mods = {};
	};

	struct Mouse
	{
		MouseButtons button = {};
		Mods         mods   = {};
		int          x      = {};
		int          y      = {};
	};

	/**
	 * @brief The type of event that actually occurs.
	 *
	 * Most of these are self explanatory, explanations will be provided for
	 * possibly obscure mechanisms.
	 */
	enum class EventType : int
	{
		NONE                  = 0,
		WINDOW_MOVED          = SDL_WINDOWEVENT_MOVED,
		WINDOW_RESIZED        = SDL_WINDOWEVENT_SIZE_CHANGED,
		WINDOW_CLOSED         = SDL_QUIT,
		WINDOW_FOCUSED        = SDL_WINDOWEVENT_FOCUS_GAINED,
		WINDOW_DEFOCUSED      = SDL_WINDOWEVENT_FOCUS_LOST,
		WINDOW_MINIMIZED      = SDL_WINDOWEVENT_MINIMIZED,
		WINDOW_MAXIMIZED      = SDL_WINDOWEVENT_MAXIMIZED,
		WINDOW_RESTORED       = SDL_WINDOWEVENT_RESTORED,
		MOUSE_BUTTON_PRESSED  = SDL_MOUSEBUTTONDOWN,
		MOUSE_BUTTON_RELEASED = SDL_MOUSEBUTTONUP,
		MOUSE_WHEEL_SCROLLED  = SDL_QUIT,
		CURSOR_MOVED          = SDL_MOUSEMOTION,
		CURSOR_LEFT           = SDL_WINDOWEVENT_LEAVE,
		CURSOR_ENTERED        = SDL_WINDOWEVENT_ENTER,
		KEY_PRESSED           = SDL_KEYDOWN,
		KEY_RELEASED          = SDL_KEYUP,
		LAYER_DESTROYED, // this one happens when a layer requests
		                 // destruction, it does NOT occur when a layer is
		                 // manually popped from a layer stack.
		LAYER_PUSHED,
	};

	struct Event
	{
		EventType type = EventType::NONE;

		//// This union MUST be used correctly or shit will go south and you
		//// might end up with corrupted memory :( We're using a union, so
		//// it's sort of like SDLs Event system, and so we don't have a
		/// huge amount of painful polymorphism that will end in us having
		/// to restart anyway.
		union {
			Position    position;
			Size        size;
			Scroll      scroll;
			Keyboard    keyboard;
			Mouse       mouse;
			const char* layer;
		};

		bool handled = false;

		Event() { std::memset(this, 0, sizeof(Event)); }
	};

	/**
	 * @beeper Sorry, my brain is done for the night. Hopefully this gives you
	 * some inspiration? :/ When I get on we can VC to go through how this shit
	 * should work (hopefully I can bring a fresh brain to that convo).
	 *
	 * Essentially my thought process on a high level is the inputMap is just an
	 * abstraction layer and nothing functional. The event system itself handles
	 * iterating through registered events and checks the input map for the key
	 * associated with the input at that given time. The state system should be
	 * related and do essentially the same thing (but we don't poll the state
	 * system).
	 *
	 * In all reality, we don't even need to build the event registry right now.
	 * The input system should work with your current Event implementation in
	 * client.cpp, the only difference is during initialization you need to
	 * register the input and store the primaryKey for reference when looping
	 * through inputs.
	 *
	 * None of this takes into account the server, if you can factor that in,
	 * great, if not we will hash that out tomorrow.
	 */

	/// @todo @beeper No clue on naming scheme here :(
	/// @todo @beeper I completely forgot what we were replacing lambdas with :(
	struct EventCallback{
	    std::string uniqueKey;
	    std::function<void()> callback;
	    size_t input;
	};

	// IDK if we can avoid a singleton here? All systems ideally need to have access to adding events.
	class EventManager{
	public:
	    /// @beeper trying to avoid having a singleton for the map if possible.
	    EventManager(InputMapper inputMap);
	    ~EventManager() = default;

	    /// @beeper Sorry, my brain is crapping out on me, the arguments may not make sense here, but we need some way to register an event :/
	    void registerEvent(std::string uniqueKey, std::function<void()> callback);
	    void registerInputEvent(std::string uniqueKey, std::function<void()> callback, std::string displayName, Event key)

	    /// @beeper this should replace the polling done in the client by looping through the vector instead.
	    void poll();

	private:
	    std::vector<EventCallback> m_events;
	    InputMapper m_inputMap;
	};

} // namespace phx::events

