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
#include <imgui_internal.h>

// Internals
#include <Quartz2/Quartz.hpp>
#include <UI.hpp>

// Standards
//...

using namespace UI;


void ChatWindow::drawEx(ImVec2 size, bool* p_open, ImVec2* padding, ImGuiWindowFlags extra_flags)
{
	// static bool focused;
	//
	//
	// ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	// ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(100,100,100,255));
	// ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(90,10,90,255));
	//
	// /***
	//  * NOTE:
	//  *	`ImGui::PushStyleVar` & `PopStyleVar` for alpha is better unified
	//  *  across children than `ImGui::SetNextWindowBgAlpha` which sets each
	//  *  "Window's" transparency to a factor of the prior existing value for one
	//  *  element. `PushStyleVar` can apply to multiple elements once.
	//  */
	// if (focused) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	// else ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.1f);
	//
	// // Window Definition
	begin(p_open, padding, flags | extra_flags);
	// focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	//
	// ImGui::SetWindowPos(ImVec2(), ImGuiCond_Once);
	// float tw = 300.0f; // terminal width
	// float th; // terminal height
	// if (focused) th = 500.0f;
	// else th = 300.0f;
	// float text_output_height = th - (textboxHeight + windowPadding->y);
	// ImGui::SetWindowSize(ImVec2(tw, th), ImGuiCond_Always);
	// //wframeheight = ImGui::GetFrameHeightWithSpacing();
	//
	//
	// ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	// ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,255,255));
	drawOutputField();
	// ImGui::PopStyleColor();
	// ImGui::PopStyleVar();
	//
	//
	// // even though pemdas applies this is just easier to read.
	drawInputField();
	//
	end();
	// ImGui::PopStyleColor();
	// ImGui::PopStyleColor();
	// ImGui::PopStyleVar(); // End Alpha Modifications
	// ImGui::PopStyleVar(); // End WindowRounding Modifications
}
