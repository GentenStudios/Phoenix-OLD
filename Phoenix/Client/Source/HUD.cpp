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

#include <Client/Client.hpp>
#include <Client/HUD.hpp>

#include <Common/Actor.hpp>
#include <Common/PlayerView.hpp>

#include <imgui.h>

using namespace phx::client;
using namespace phx;

HUD::HUD(gfx::Window* window, entt::registry* registry, entt::entity player)
    : gfx::Overlay("HUD"), m_window(window), m_registry(registry),
      m_player(player)
{
}

void HUD::onAttach() {}
void HUD::onDetach() {}
void HUD::onEvent(events::Event& e) {}

void HUD::tick(float dt)
{
	voxels::Map* map = m_registry->get<PlayerView>(m_player).map;
	ImGui::SetNextWindowPos(
	    {m_window->getSize().x / 2 - 250, m_window->getSize().y - 100},
	    ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 50), ImGuiCond_Once);

	ImGui::Begin("HUD", nullptr,
	             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	{
		std::string targetDisplay;
		auto        target =
		    ActorSystem::getTarget(m_registry, m_player).getCurrentPosition();
		target.floor();
		auto targetBlock = map->getBlockAt(target);
		if (targetBlock->category == voxels::BlockCategory::SOLID)
		{
			targetDisplay = targetBlock->displayName;
		}
		ImGui::Text("%s", targetDisplay.c_str());

		ImGui::Text("Hand: %s",
		            m_registry->get<Hand>(m_player).hand->displayName.c_str());
	}
	ImGui::End();
}