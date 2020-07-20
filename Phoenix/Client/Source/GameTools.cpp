// Copyright 2019-20 Genten Studios
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

#include <Client/GameTools.hpp>

#include <Common/Actor.hpp>
#include <Common/Position.hpp>

#include <imgui.h>

using namespace phx::client;
using namespace phx;

GameTools::GameTools(bool* followCam, entt::registry* registry,
                     entt::entity player)
    : Overlay("GameTools"), m_followCam(followCam), m_registry(registry),
      m_player(player)
{
}

void GameTools::onAttach()
{
	m_sensitivity        = Settings::get()->getSetting("camera:sensitivity");
	m_currentSensitivity = m_sensitivity->value();
}

void GameTools::onDetach() {}

void GameTools::onEvent(events::Event& e) {}

void GameTools::tick(float dt)
{
	ImGui::Begin("Phoenix");
	if (ImGui::CollapsingHeader("Game Tools"))
	{
		ImGui::Checkbox("Follow Camera", m_followCam);

		int i = m_currentSensitivity;
		ImGui::SliderInt("cam sensitivity", &i, 0, 100);
		if (i != m_currentSensitivity)
		{
			m_currentSensitivity = i;
			m_sensitivity->set(m_currentSensitivity);
		}

		ImGui::Text("X: %f\nY: %f\nZ: %f",
		            m_registry->get<Position>(m_player).position.x,
		            m_registry->get<Position>(m_player).position.y,
		            m_registry->get<Position>(m_player).position.z);

		ImGui::Text("Block in hand: %s",
		            m_registry->get<Hand>(m_player).hand->displayName.c_str());
	}
	ImGui::End();
}
