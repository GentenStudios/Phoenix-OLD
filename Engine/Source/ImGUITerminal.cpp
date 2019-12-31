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

#include <Quartz2/ImGUITerminal.hpp>
#include <vector>
#include <sstream>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

using namespace q2;

/*
 * Limitations of ImGui:
 *	No element specific styling, the styling effects every single element.
 *  Some widgets must be made from scratch because they don't exist outright.
 *
 * Extenious Notes On ImGui Function:
 *  All root (and child) window delcarations are rendered after the `End`
 *  or respective API call, and thus any variable defined before those points
 *  even if left uninitialized before the `Begin` call. This information is
 *	particularly handy when considering that most window size setters rely on
 *  data that may not yet available such as accomidations for inner elements
 *  in the case of responsive styling.
 */

// Tetramental Terminal... Funcing ImGUI's easy but terrible.
std::vector<std::string> comm_history = {"Hello! Welcome!"};
std::string buffer = "";

// This is a ImGuiInputTextCallback
int callback(ImGuiInputTextCallbackData *data){
  switch(data->EventFlag){
    case ImGuiInputTextFlags_CallbackAlways:
		  //comm_history.push_back("CallbackAlways");
			if (data->EventChar == 0x0d) {
				comm_history.push_back(buffer);
				buffer.clear();
			}
		break;

		// Just in case we need these.
		case ImGuiInputTextFlags_CallbackCompletion:
			comm_history.push_back(std::string("CallbackCompletion"));
		break;

		case ImGuiInputTextFlags_CallbackHistory:
			comm_history.push_back(std::string("CallbackHistory"));
		break;

		// Filtered by imgui_stdlib implementation.
		// case ImGuiInputTextFlags_CallbackCharFilter:
		// 	std::cout << "CallbackCharFilter\n";
		// break;
  };
	//comm_history.push_back(std::string("Contents: ") + std::string(data->Buf));
	// comm_history.push_back()
	return 1;
}


ImGuiWindowFlags WindowFlags = (
	ImGuiWindowFlags_NoTitleBar |
	ImGuiWindowFlags_NoResize |
	ImGuiWindowFlags_NoMove |
	ImGuiWindowFlags_NoCollapse |
	ImGuiWindowFlags_NoSavedSettings |
	ImGuiWindowFlags_NoScrollbar |
	ImGuiWindowFlags_NoScrollWithMouse
);

void Terminal::draw(){
	// stage migration to Terminal Class or parameter value

	// Transparency toggle holder
	static bool focused;
	static float textbox_height; // this will be the font height
	static float wframeheight;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(100,100,100,255));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(90,10,90,255));

	/***
	 * NOTE:
	 *	`ImGui::PushStyleVar` & `PopStyleVar` for alpha is better unified
	 *  across children than `ImGui::SetNextWindowBgAlpha` which sets each
	 *  "Window's" transparency to a factor of the prior existing value for one
	 *  element. `PushStyleVar` can apply to multiple elements once.
	 */
	if (focused) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	else ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.1f);


	// Window Definition
	ImGui::Begin("Terminal", NULL, WindowFlags);
	focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

	// Window Dimensions
	ImGui::SetWindowPos(ImVec2(),  ImGuiCond_Once);
	float tw = 300.0f; // terminal width
	float th; // terminal height
	if (focused) th = 500.0f;
	else th = 300.0f;
	float text_output_height = th - (textbox_height + wframeheight);
	ImGui::SetWindowSize(ImVec2(tw, th), ImGuiCond_Always);
	wframeheight = ImGui::GetFrameHeightWithSpacing();


	// Controls transparency for children.
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	ImGui::BeginChild("Output", ImVec2(tw, text_output_height) /*, ImGuiWindowFlags() */ );
	// ImVec4 previous = ImGui::GetStyleColorVec4(ImGuiCol_Text);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,255,255));
	//ImGui::PushStyleColor(ImGuiCol_Text, previous);
	for (auto it = comm_history.cbegin(); it != comm_history.cend(); it++)
	    ImGui::Text("%s", it->c_str());
	ImGui::PopStyleColor();
	ImGui::EndChild();
	ImGui::PopStyleVar();




	ImGui::PushItemWidth(tw);
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2()); // (0,0)
	//ImGui::SetNextWindowBgAlpha(0.5f);
	// start textbox input at the bottom.
	bool append_line = ImGui::InputText("\0", &buffer,
		// * These can be used in conjunction for extra functionality *
		// NOTE: The Callback doesn't need to be used for line by line input.
	  ImGuiInputTextFlags(
			ImGuiInputTextFlags_EnterReturnsTrue // | // makes append_line true on enter
		  //ImGuiInputTextFlags_CallbackCompletion | // callback on tab
		  //ImGuiInputTextFlags_CallbackHistory | // callback on up/down
			//ImGuiInputTextFlags_CallbackAlways // workaround because the callback isn't working.
		  //ImGuiInputTextFlags_AllowTabInput // can't be used with cb_completion
		), // << this one I'm not sure about yet.
		&callback,
	  NULL // Can be a user data object but unnecessary here.
	);
	textbox_height = ImGui::GetFrameHeight(); // better than GetWindowHeight
	//ImGui::SetWindowPos(ImVec2(tw, th - textbox_height - 60), ImGuiCond_Always);
	if (append_line) {
		comm_history.push_back(std::string(buffer));
		buffer.clear(); // clear our input buffer && subsequently the text box.
		// sets the kb focus to the last element drawn (in our case the input box.)
		// we have to do this because otherwise when we press enter the text box
		// by default loses focus.
		ImGui::SetKeyboardFocusHere(-1);
	}
	//ImGui::PopStyleVar(); // Window Padding For the Text Bar


	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(); // End Alpha Modifications
	ImGui::PopStyleVar(); // End WindowRounding Modifications

	// PushStyleCol() mid frame style color changes perhaps try every draw change
	// and reset before element render finishes
	// send chat window to back for hovering content visibility / access.
	// ImGui::FocusWindow(ImGuiWindow* window)
	// ImGui::SetWindowFocus() // focueses on the current window.
	// ImGui::SetKeyboardFocusHere(int offset) // denote
	// ImGui::BringWindowToDisplayBack(ImGuiWindow* window)
	// ImRect ImGui::GetViewportRect() // usefull for placing the chat window.
	// ImGui::SetItemAllowOverlap() // allow overlap on current item
	// bool ImGui::IsItemEdited() // Returns true on window edit.
	// bool ImGui::IsWindowHovered() // for Austin's alpha toggling
	// ImGui::PushAllowKeyboardFocus(bool allow_keyboard_focus) // check this out.
	// void ImGui::BeginGroup() // investigate for hovering sake
	// ImGui::SameLine(float offset_from_start_x, float spacing_w) // for making things horizontal
	// float ImGui::GetTextLineHeight() // just in case I need to write my own selectable text box.
	// void ImGui::SetClipboardText(const char* text) // also for the above case
	// void  AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL); // see prior comments.
}
