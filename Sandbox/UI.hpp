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

namespace UI {
	class ChatWindow : public q2::ImGuiHelpers::BasicTerminal
	{
		protected:
			// I was thinking this would be inherited and the other class
			// would be able to grab it outright but appearently not. Besides, it's
			// not like the code would look any better as a result. While I could
			// use a singleton to achive this on an inheritance basis by abusing
			// the runtime checking employed by "virtual" I doubt this is a feature
			// anyone wants so I'm leaving it like it is for now with the
			// variable being passed as an argument to the begin function within
			// this classes draw function.
			static const ImGuiWindowFlags defaultFlags = ImGuiWindowFlags(
				// Handled by NoDecoration
				// ImGuiWindowFlags_NoCollapse |
				// ImGuiWindowFlags_NoResize |
				// ImGuiWindowFlags_NoTitleBar |
				// ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoSavedSettings |

				// because we override the default behavior for these settings
				ImGuiWindowFlags_NoScrollWithMouse |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoBringToFrontOnFocus
			);

			float unselectedTransparency = 0.3f;
			bool renderFocus = false;
		public:
			using BasicTerminal::BasicTerminal;
			inline void focus(){renderFocus=true;}; // external focus events.
			void drawEx(bool* p_open, ImGuiWindowFlags flags);

	};
};
