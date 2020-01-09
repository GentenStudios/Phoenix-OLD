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

#pragma once

#include <Quartz2/Quartz.hpp>
#include <imgui.h>

/// @brief Contains all UI elements specifically designed for Phoenix.
namespace UI
{
	// @brief A restyle of the standard BasicTerminal for Phoenix
	class ChatWindow : public q2::ImGuiHelpers::BasicTerminal
	{
		/// @privatesection
	private:
		/// @brief The current external focus event state.
		bool renderFocus = false;

		/// @protectedsection
	protected:
		/// @brief used when setting the unselected chat terminal transparency.
		float unselectedTransparency = 0.3f;

		/// @publicsection
	public:
		using BasicTerminal::BasicTerminal;

		/// @brief Sets the external focus event flag for the current
		///   terminal / the chat window. Upon call, chat's input window will
		///   get focus regardless of player ingame state.
		inline void focus() { renderFocus = true; };

		void drawEx(bool* p_open, ImGuiWindowFlags flags);
	};
}; // namespace UI
