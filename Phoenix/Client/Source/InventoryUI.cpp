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
#include <Client/InventoryUI.hpp>

#include <imgui.h>

using namespace phx::client;
using namespace phx;
InventoryUI::InventoryUI(gfx::Window* window, gfx::FPSCamera* camera,
                         voxels::Inventory*    inventory,
                         voxels::ItemReferrer* referrer)
    : gfx::Overlay("Inventory"), m_window(window), m_inventory(inventory),
      m_camera(camera), m_referrer(referrer)
{
}

void InventoryUI::onAttach()
{
	m_active = true;
	m_camera->enable(false);
}

void InventoryUI::onDetach()
{
	m_active = false;
	m_camera->enable(true);
}

void InventoryUI::onEvent(events::Event& e)
{
	if (e.type == events::EventType::KEY_PRESSED &&
	    (e.keyboard.key == events::Keys::KEY_I ||
	     e.keyboard.key == events::Keys::KEY_ESCAPE))
	{
		Client::get()->popLayer(this);
		e.handled = true;
	}
}

void InventoryUI::tick(float dt)
{
	ImGui::SetNextWindowPos({m_window->getSize().x / 2, 100});
	ImGui::Begin("Inv Holding", nullptr, ImGuiWindowFlags_NoResize);
	{
		if (m_holding.type == nullptr)
		{
			ImGui::Button("", {50, 50});
		}
		else
		{
			ImGui::Button(m_holding.type->displayName.c_str(), {50, 50});
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos({m_window->getSize().x / 2 - WIDTH / 2,
	                         m_window->getSize().y / 2 - HEIGHT / 2});
	ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));

	ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_NoResize);
	for (size_t i = 0; i < m_inventory->getSize(); i++)
	{
		if ((i % 10) != 0)
		{
			ImGui::SameLine();
		}
		const voxels::Item item = m_inventory->getItem(i);
		ImGui::PushID(i);
		if (item.type == nullptr)
		{
			if (ImGui::Button("", {50, 50}))
			{
				LOG_DEBUG("Inv") << "Click";
				if (m_holding.type != nullptr)
				{
					m_inventory->addItem(i, m_holding);
					m_holding = {nullptr, nullptr};
				}
			}
		}
		else
		{
			if (ImGui::Button(item.type->displayName.c_str(), {50, 50}))
			{
				if (m_holding.type == nullptr)
				{
					m_holding = m_inventory->removeItem(i);
				}
				else
				{
					auto tmp  = m_holding;
					m_holding = m_inventory->removeItem(i);
					m_inventory->addItem(i, tmp);
				}
			}
		}
		ImGui::PopID();
	}
	ImGui::End();
	ImGui::SetNextWindowPos({m_window->getSize().x - CREATIVE_WIDTH,
	                         m_window->getSize().y / 2 - CREATIVE_HEIGHT / 2});
	ImGui::SetNextWindowSize(ImVec2(CREATIVE_WIDTH, CREATIVE_HEIGHT));

	ImGui::Begin("Creative Inventory", nullptr, ImGuiWindowFlags_NoResize);
	{
		for (std::size_t i = 2; i < m_referrer->referrer.size(); i++)
		{
			if ((i % 5) != 0)
			{
				ImGui::SameLine();
			}
			if (ImGui::Button(m_referrer->items.get(i)->displayName.c_str(),
			                  {50, 50}))
			{
				m_holding = {m_referrer->items.get(i), nullptr};
			}
		}
	}
	ImGui::End();
}
