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

// Externals
#include <imgui.h>

// Internals
#include <Quartz2/Quartz.hpp>
#include <UI.hpp>

// Standards
//...

using namespace UI;


void ChatWindow::drawEx(ImVec2* size, ImVec2* pos, ImVec2* padding, bool* p_open, ImGuiWindowFlags extra_flags)
{
	static bool focused;

	/***
	 * NOTE:
	 *	`ImGui::PushStyleVar` & `PopStyleVar` for alpha is better unified
	 *  across children than `ImGui::SetNextWindowBgAlpha` which sets each
	 *  "Window's" transparency to a factor of the prior existing value for one
	 *  element. `PushStyleVar` can apply to multiple elements once.
	 */
	if (focused) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	else ImGui::PushStyleVar(ImGuiStyleVar_Alpha, unselectedTransparency);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(100,100,100,255));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(90,10,90,255));

	// Window Definition
	begin(NULL, pos, padding, p_open, currentFlags | extra_flags); //just in case
	focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

	// embedded default size
	ImVec2 s = (size != NULL) ? (*size) : ImVec2(400, 600);
	ImGui::SetWindowSize(ImVec2(s.x, ( focused ? s.y : (s.y - 200.0f) )), ImGuiCond_Always);
	// Shrink from top down, by default it's reverse because windows are renderd from
	// the top left.
	if (pos != NULL)
		ImGui::SetWindowPos(ImVec2(pos->x, (focused ? pos->y : (pos->y + 200.0f) )), ImGuiCond_Always);


	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	drawOutputField();
	ImGui::PopStyleVar();

	drawInputField();

	end();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(); // End WindowRounding Modifications
	ImGui::PopStyleVar(); // End Alpha Modifications
}
