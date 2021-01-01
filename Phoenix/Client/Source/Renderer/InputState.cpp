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

#include <Client/Renderer/InputState.hpp>

#include <SDL.h>

using namespace phx::render;

bool InputState::getKeyState(const events::Keys& key) const
{
	return SDL_GetKeyboardState(nullptr)[static_cast<SDL_Scancode>(key)];
}

bool InputState::getModstate(const events::Mods& key) const
{
	return SDL_GetModState() | static_cast<SDL_Keymod>(key);
}

bool InputState::getMouseState(const events::MouseButtons& button) const
{
	return SDL_GetMouseState(nullptr, nullptr) &
	       SDL_BUTTON(static_cast<int>(button));
}

bool InputState::getWindowFocusState() const
{
	// https://stackoverflow.com/a/24930075 <- we use INPUT_FOCUS since it
	// checks if window is active.
	return SDL_GetWindowFlags(m_window) & SDL_WINDOW_INPUT_FOCUS;
}

void InputState::hideCursor() { SDL_ShowCursor(SDL_DISABLE); }

void InputState::showCursor() { SDL_ShowCursor(SDL_ENABLE); }

bool InputState::isCursorHidden() const { return SDL_ShowCursor(SDL_QUERY); }

phx::math::vec2 InputState::getCursorPosition() const
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return {x, y};
}

void InputState::setCursorPosition(const math::vec2& pos)
{
	SDL_WarpMouseInWindow(m_window, pos.x, pos.y);
}

void InputState::setMouseRelativeMode(bool enable)
{
	SDL_SetRelativeMouseMode(enable ? SDL_TRUE : SDL_FALSE);
}
