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

#include <Phoenix/Graphics/ImGuiExtensions.hpp>
#include <imgui.h>

#include <glad/glad.h>

using namespace phx::client;
using namespace phx;

DebugOverlay::DebugOverlay() : Overlay("DebugOverlay")
{
	SDL_DisplayMode current;

	for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
	{
		if (SDL_GetCurrentDisplayMode(i, &current) == 0)
		{
			if (current.refresh_rate > m_maxSampleRate)
				m_maxSampleRate = current.refresh_rate;
		}
	}
}

void DebugOverlay::onAttach() {}

void DebugOverlay::onDetach() {}

void DebugOverlay::onEvent(events::Event& e) {}

void DebugOverlay::tick(float dt)
{
	ImGui::Begin("Phoenix");

	if (ImGui::CollapsingHeader("Graphics Information"))
	{
		if (ImGui::Checkbox("Wireframe", &m_wireframe))
		{
			if (m_wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		ImGui::Text("Frame Time: %.2f ms/frame\n", dt * 1000.f);
		ImGui::Text("FPS: %d\n", static_cast<int>(1.f / dt));

		ImGui::SliderInt("Debug Sample Rate", &m_sampleRate, 1, 60);
		ImGui::Checkbox("Pause Debug Graph", &m_pauseSampling);

		if (m_time % m_sampleRate == 0 && !m_pauseSampling)
		{
			ImGui::PlotVariable("Frame Time: ", dt * 1000.f);
		}
		else
		{
			ImGui::PlotVariable("Frame Time: ", FLT_MAX);
		}
	}

	ImGui::End();

	++m_time;
	if (m_time >= 3600)
		m_time = 0;
}
