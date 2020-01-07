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

//#include <vector>
#include <cmath>

#include <imgui.h>
#include <imgui_internal.h> // for nav inputs
#include <misc/cpp/imgui_stdlib.h>

using namespace q2::ImGuiHelpers;

// Usefull code snippits:
// ======================
//
// // ImGuiWindow* TitleBarHeight() and MenuBarHeight() return 0 when their
// // respective ui elements aren't rendered.
// const float decoration_up_height = window->TitleBarHeight() +
// window->MenuBarHeight();
//
// ImGuiWindow* window = FindWindowByName(name); // Find or create (also has
// ByID)

/*
 * Extenious Notes On ImGui Function:
 *  All root (and child) window delcarations are rendered after the `End`
 *  or respective API call, and thus any variable defined before those points
 *  even if left uninitialized before the `Begin` call. This information is
 *	particularly handy when considering that most window size setters rely on
 *  data that may not yet available such as accomidations for inner elements
 *  in the case of responsive styling.
 */

// const char* inputFieldPrefix = "_Input";
// const char* outputFieldPrefix = "_Output";

// This is a ImGuiInputTextCallback
// TODO: add terminal history & hook for autocomplete.
int callback(ImGuiInputTextCallbackData* data)
{
	// switch(data->EventFlag){
	//   case ImGuiInputTextFlags_CallbackAlways:
	// 	  //comm_history.push_back("CallbackAlways");
	// 		if (data->EventChar == 0x0d) {
	// 			comm_history.push_back(buffer);
	// 			buffer.clear();
	// 		}
	// 	break;
	//
	// 	// Just in case we need these.
	// 	case ImGuiInputTextFlags_CallbackCompletion:
	// 		comm_history.push_back(std::string("CallbackCompletion"));
	// 	break;
	//
	// 	case ImGuiInputTextFlags_CallbackHistory:
	// 		comm_history.push_back(std::string("CallbackHistory"));
	// 	break;
	//
	// 	// Filtered by imgui_stdlib implementation.
	// 	// case ImGuiInputTextFlags_CallbackCharFilter:
	// 	// 	std::cout << "CallbackCharFilter\n";
	// 	// break;
	// };
	// comm_history.push_back(std::string("Contents: ") +
	// std::string(data->Buf));
	// comm_history.push_back()
	return 1;
}

void BasicTerminal::flush()
{
	std::string buf       = cout.str();
	int         bufSize   = buf.length();
	int         cacheSize = cache.length();

	// ceiling this so we don't run into an infinite loop with extremely small
	// targetOutputSizes although it should never happen realistically because
	// I made sure this is alligned in kilobytes of memory.
	int hundredthSize = static_cast<int>(std::ceil(targetOutputSize / 100));
	// Just for backup the while loop seeks a slightly larger segment
	// so after we clear up
	int tenthSize = static_cast<int>(std::floor(targetOutputSize / 10));

	// flush content from buffer into output
	if (bufSize > 0)
	{
		// When our input buffer is larger than our output cache just replace
		// the output cache with a clip of the input buffer's content that
		// is the correct size.
		if (bufSize > targetOutputSize)
			cache = buf.substr(bufSize - targetOutputSize, bufSize);

		// otherwise just append to the end.
		cache.append(buf);
	}

	if (cacheSize > targetOutputSize)
	{
		// then ensure the output content fits with our bounds
		do
		{
			// IDK if this is done programatically or not, best practice to do
			// once, reuse return value since this is up to system
			// implementation I believe.
			cacheSize = cache.length();

			// Auto cut at one hundredth the targetSize or newline, which ever
			// it hits first.
			for (int l = tenthSize; (l > 0) && cache[l] != '\n'; l++)
			// for (auto l=0; (l < hundredthSize && cache[l] != "\n" && l <
			// cacheSize; l++)
			{
				// move the buffer back
				// for(auto l2=0; l2 <= bufSize-l+1; l2++)
				// 	cache[l2] = cache[l2+l]
				// may be faster to use more memory and hope fewer malloc calls
				// to the kernel results in a faster over-all experience.

				// leave cache unchanged if l < cache.length() triggers first.
				if (l < cacheSize)
					cache = cache.substr(l, cacheSize);
			}
		} while (cacheSize > targetOutputSize - hundredthSize);
	}

	// clear the output buffer and reset the cout internal object.
	buf.clear();
	cout.str(buf);
}

// NOTE: This inline is okay because it's only used within this file.
inline void renderText(std::string* text)
{
	// TODO:
	//   Implement ANSI escape sequences for color because this is
	//   supposed to be a terminal after all.
	//
	// TODO:
	//   Implement wrapping vs Non-Wrapping text and investigate getting the
	//   line width for characters to better implement the output cutoff /
	//   string buffer allocator.
	//
	//   This may be of use for that research.
	//   https://github.com/emoon/ProDBG/issues/304
	//
	//   Also, this coincides with the ANSI escape sequence rendering,
	//   at some point I need to test and see whether each of those functions
	//   write inline and if they automatically know how much text was
	//   previously written to the screen.
	//
	// TODO:
	//   ImDoesn't have a function for printing complete strings / unformatted
	//   text without using a printf loop. So we should probably implement one
	//   while we're still using ImGui and push it to ImGui so other people can
	//   benefit from it. It would save a lot of overhead per frame on the
	//   widget since we're printing multiple times per frame due to the newline
	//   / ANSI escape sequence parse.
	//
	// // PushTextWrapPos(); // possibly good for something

	// ImGui:TextEx(start*, end*, flags);
	// with ImGui::PushStyleCol; & pop to achieve ansi escape sequences.
	// Pass to our render function as to not clutter the configuration here.
	// int begin = 0, current = 0;
	// int cacheSize = cache.length();
	// while (current < cacheSize)
	// {
	// 	switch(cache[current])
	// 	{
	// 		case '\n':
	// 			std::string line = cache.substr(begin, current-begin-1);
	// 			// skip newline because on each subsequent call to  ImGui::Text
	// 			// or ImGui::TextWrapped, it automatically puts next
	// 			//  things on the line for you.
	//
	// 			begin = current + 1;
	//
	// 		default:
	// 			current++;
	// 	}
	// }
	//
	// // Prints the last line if it doesn't contain a newline at the end.
	// // Basically just edge case handling.
	// if (begin != current && current == cache.length())
	// 	renderLine(cache.substr(begin, current));

	// For now this function actually handles newlines so it's a fix.
	ImGui::TextWrapped("%s", (*text).c_str());
};

void BasicTerminal::drawOutputField(ImGuiWindowFlags flags)
{
	// May need later for text scrolling implementatinos.
	// ImGuiContext* g = ImGui::GetCurrentContext();
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	// render the text output block with accommodation for input box height.
	ImGui::BeginChild(
	    outputWindowName,
	    ImVec2(window->SizeFull.x - (window->WindowPadding.x * 2.0f),
	           window->SizeFull.y - (window->WindowPadding.y * 2.0f) -
	               ImGui::GetFrameHeightWithSpacing() - // accomodate for input
	                                                    // field
	               // TitleBarHeight() and MenuBarHeight() return 0 when their
	               // respective ui elements aren't rendered.
	               window->TitleBarHeight() - window->MenuBarHeight()),
	    flags);

	renderText(&cache);

	ImGui::EndChild();
}

// Fuck this is going to be annoying to make threadsafe
void BasicTerminal::drawInputField()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	// InputText is a frame object so we have to use push item width to adjust
	// it.
	// TODO: investigate label width changing or outright removal becaouse right
	//   now we don't have a way to remove it, the data for it is just sitting
	//   off screen to the right.
	ImGui::PushItemWidth(ImGui::GetWindowWidth() -
	                     (window->WindowPadding.x * 2.0f));
	bool append_line = ImGui::InputText(
	    "\0",         // make sure we don't have any trailing text to the right.
	    &inputBuffer, // Our output buffer object.

	    // * These can be used in conjunction for extra functionality *
	    // NOTE: The Callback doesn't need to be used for line by line input.
	    ImGuiInputTextFlags(
	        // Makes the widget return true on enter.
	        ImGuiInputTextFlags_EnterReturnsTrue
	        // ImGuiInputTextFlags_CallbackCompletion | // callback on tab
	        // ImGuiInputTextFlags_CallbackHistory | // callback on up/down

	        // Can't be used with cb_completion
	        // ImGuiInputTextFlags_AllowTabInput
	        ),
	    &callback,

	    NULL // Can be a user data object but unnecessary here.
	);
	ImGui::PopItemWidth();

	if (append_line)
	{
		// create a copy of the input buffer string to send to our command
		// capture.
		onCommand(std::string(inputBuffer));

		// clear our input buffer && subsequently the text box.
		inputBuffer.clear();

		// sets the kb focus to the last element drawn (in our case the input
		// box.) we have to do this because otherwise when we press enter the
		// text box by default loses focus.
		ImGui::SetKeyboardFocusHere(-1);
	}
}
