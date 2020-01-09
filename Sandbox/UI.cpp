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

using namespace ui;

// hacked from the ImGui source
static const float WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER = 2.00f;
static void        ForceUpdateMouseWheel()
{
	ImGuiContext* g = ImGui::GetCurrentContext();

	// Reset the locked window if we move the mouse or after the timer elapses
	if (g->WheelingWindow != NULL)
	{
		g->WheelingWindowTimer -= g->IO.DeltaTime;
		if (g->WheelingWindowTimer <= 0.0f)
		{
			g->WheelingWindow      = NULL;
			g->WheelingWindowTimer = 0.0f;
		}
	}
}

void ChatWindow::drawEx(bool* p_open, ImGuiWindowFlags flags)
{
	ImGuiContext* g = ImGui::GetCurrentContext();
	static bool   focused;

	/***
	 * NOTE:
	 *	`ImGui::PushStyleVar` & `PopStyleVar` for alpha is better unified
	 *  across children than `ImGui::SetNextWindowBgAlpha` which sets each
	 *  "Window's" transparency to a factor of the prior existing value for one
	 *  element. `PushStyleVar` can apply to multiple elements once.
	 */
	if (focused)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	}
	else
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, unselectedTransparency);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(100, 100, 100, 255));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(90, 10, 90, 255));

	// Window Definition
	begin(p_open,
	      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
	          ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove |
	          ImGuiWindowFlags_NoBringToFrontOnFocus | flags);

	// Because the focused variable is static, and we can use it as a state
	// that's saved across function calls, we can infer when
	// there's a change in the ImGui focused state and thus implement the
	// scroll on focus feature. Currently this is focusing on the top most
	// segment of the display that was last active.
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
	    !focused)
	{
		ImGui::SetScrollY(ImGui::GetScrollY() - 200.0f);
	}
	else if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
	         focused)
	{
		ImGui::SetScrollY(ImGui::GetScrollY() + 200.0f);
	}

	focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

	// if we're focused anywhere within the window this captures the mouse
	// wheeling / scroll feature. Without this no scrolling happens unless
	// the mouse is hovering over the window.
	// NOTE:
	//   This is what I have so far, it's not working obviously.
	//   I looked all over the ImGui source and couldn't find much else.
	//   There has to be a way to capture the MouseWheel when it's not
	//   hovering over the target window.
	// TODO:
	//   Make this work. Please lol. I'm tired of working on it.
	//
	// if (focused)
	// {
	// 	g->WheelingWindow = window;
	// 	g->WheelingWindowRefMousePos = g->IO.MousePos;
	// 	g->WheelingWindowTimer = WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER;
	// }
	// ForceUpdateMouseWheel();
	// if (focused && (g->IO.MouseWheel < 0.0f))
	// 	ImGui::SetScrollY(ImGui::GetScrollY() - ( ImGui::GetFontSize() *
	// g->IO.MouseWheel)); if (focused && (g->IO.MouseWheel > 0.0f))
	// 	ImGui::SetScrollY(ImGui::GetScrollY() + ( ImGui::GetFontSize() *
	// g->IO.MouseWheel));

	// Embedded default size
	// Shrink from top down, by default it's reverse because windows are renderd
	// from the top left.
	ImVec2 size = ImVec2(400, (focused ? 600 : 400));
	ImGui::SetWindowSize(ImVec2(size.x, size.y), ImGuiCond_Always);

	// Set to the bottom right with accommodation for size changes
	// This only works because we don't have a title bar. Otherwise we'd have
	// to factor that in as well.
	ImGui::SetWindowPos(ImVec2(0, g->IO.DisplaySize.y - size.y),
	                    ImGuiCond_Always);

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
	drawOutputField(ImGuiWindowFlags_NoScrollbar);
	ImGui::PopStyleVar();

	if (m_renderFocus)
	{
		ImGui::SetKeyboardFocusHere(1);
		renderFocus = false;
	}

	drawInputField();

	end();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(); // End WindowRounding Modifications
	ImGui::PopStyleVar(); // End Alpha Modifications
}
