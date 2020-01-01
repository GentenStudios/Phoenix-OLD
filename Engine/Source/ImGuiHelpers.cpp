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

#include <Quartz2/ImGuiHelpers.hpp>

#include <vector>
#include <sstream>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

using namespace q2::ImGuiHelpers;

/*
 * Extenious Notes On ImGui Function:
 *  All root (and child) window delcarations are rendered after the `End`
 *  or respective API call, and thus any variable defined before those points
 *  even if left uninitialized before the `Begin` call. This information is
 *	particularly handy when considering that most window size setters rely on
 *  data that may not yet available such as accomidations for inner elements
 *  in the case of responsive styling.
 */

//const char* inputFieldPrefix = "_Input";
//const char* outputFieldPrefix = "_Output";

std::vector<std::string> comm_history = {"Test!"};
std::string buffer("");

// This is a ImGuiInputTextCallback
// TODO: add terminal history & function to enable autocomplete.
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


// NOTE: Subdue vtable linker errors.
// TODO: Throw NotImplemented Errors when these are called if this is added
//   to the engine for any reason.
void TerminalBase::drawInputField(){};
void TerminalBase::drawOutputField(){};
void TerminalBase::drawEx(ImVec2 size, bool* p_open, ImVec2* padding, ImGuiWindowFlags flags){};
void TerminalBase::onCommand(){};


void BasicTerminal::drawInputField()
{
	// InputText is a frame object so we have to use push item width to adjust it.
	ImGui::PushItemWidth(ImGui::GetWindowWidth() - ( currentPadding->x * 2.0f )  );
	bool append_line = ImGui::InputText(
		"\0", // make sure we don't have any trailing text to the right.
		&buffer, // Our output buffer object.

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
	textboxHeight = ImGui::GetFrameHeight() * 2.0f;

	if (append_line) {
		onCommand();
		buffer.clear(); // clear our input buffer && subsequently the text box.
	}

	// sets the kb focus to the last element drawn (in our case the input box.)
	// we have to do this because otherwise when we press enter the text box
	// by default loses focus.
	if (append_line || renderFocus){
		ImGui::SetKeyboardFocusHere(-1);
		renderFocus = false;
	}
	ImGui::PopItemWidth();
}

void BasicTerminal::drawOutputField()
{
	ImVec2 size = ImGui::GetWindowSize();

	// TODO:
	//   Implement ANSI escape sequences for color because this is
	//   supposed to be a terminal after all.

	// render the text output block with accommodation for
	ImGui::BeginChild(outputName, ImVec2(
		size.x - ( currentPadding->x * 2.0f ),
		size.y - textboxHeight - ( currentPadding->y * 2.0f )
	));

	// PushTextWrapPos(); // possibly good for something
	for (auto it = comm_history.cbegin(); it != comm_history.cend(); it++)
	    ImGui::Text("%s", it->c_str());

	ImGui::EndChild();
}


void BasicTerminal::drawEx(ImVec2 size, bool* p_open, ImVec2* padding, ImGuiWindowFlags extra_flags)
{
	// Window Definition (remember to use the terminal class' begin)
	begin(p_open, padding, flags | extra_flags);
	ImGui::SetWindowSize(size);
	drawOutputField();
	drawInputField();
	end();
}

void BasicTerminal::onCommand()
{
	comm_history.push_back(std::string(buffer));
}
