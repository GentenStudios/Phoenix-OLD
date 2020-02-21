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

#include <Phoenix/DebugOverlay.hpp>

using namespace phx::client;
using namespace phx;

DebugOverlay::DebugOverlay() : Overlay("DebugOverlay") {}

DebugOverlay::~DebugOverlay() {}

void DebugOverlay::onAttach() {}

void DebugOverlay::onDetach() {}

void DebugOverlay::onEvent(events::Event& e) {}

void DebugOverlay::tick(float dt)
{
	const float DISTANCE = 10.0f;
	const int   corner   = 1;
	static bool p_open   = true;

	ImGuiIO& io = ImGui::GetIO();

	const ImVec2 window_pos =
	    ImVec2(io.DisplaySize.x - DISTANCE, DISTANCE * 2.5f);
	const ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

	ImGui::SetNextWindowBgAlpha(0.3f);

	ImGui::Begin("Debug Overlay Hint", &p_open,
	             (corner != -1 ? ImGuiWindowFlags_NoMove : 0) |
	                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
	                 ImGuiWindowFlags_AlwaysAutoResize |
	                 ImGuiWindowFlags_NoSavedSettings |
	                 ImGuiWindowFlags_NoFocusOnAppearing |
	                 ImGuiWindowFlags_NoNav);


	ImGui::End();

	ImGui::Begin("Debug Tools");
	if (ImGui::Checkbox("Wireframe", &m_wireframe))
	{
		if (m_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	ImGui::Text("Frame Time: %f\n", dt);
	ImGui::Text("Frames Per Second: %d\n", static_cast<int>(1 / dt));

	ImGui::End();
}
