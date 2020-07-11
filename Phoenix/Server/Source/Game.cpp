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

#include <Server/Game.hpp>
#include <Server/User.hpp>

#include <Common/Actor.hpp>
#include <Common/PlayerView.hpp>

#include <thread>

using namespace phx;
using namespace phx::server;

Game::Game(entt::registry* registry, bool* running, phx::server::net::Iris* iris,
           const std::string& save)
    : m_registry(registry), m_running(running), m_iris(iris),
      m_map(voxels::Map(save, "map1"))
{
	m_commander = new Commander(m_iris);
}

void Game::registerAPI(cms::ModManager* manager)
{
	m_commander->registerAPI(manager);
}

void Game::run()
{
	while (m_running)
	{
		// Wait for a new input bundle from the network
		if (m_iris->stateQueue.empty())
		{
			// This just keeps the CPU from spinning
			std::this_thread::sleep_for(1_ms);
			continue;
		}
		net::StateBundle m_currentState = m_iris->stateQueue.pop();

		// Process everybody's input first
		for (const auto& state : m_currentState.states)
		{
			auto       player = m_registry->get<Player>(state.first);
			math::vec3 pos = m_registry->get<Position>(player.actor).position;
			const math::vec3i oldPos(
			    static_cast<int>(pos.x) / voxels::Chunk::CHUNK_WIDTH,
			    static_cast<int>(pos.y) / voxels::Chunk::CHUNK_HEIGHT,
			    static_cast<int>(pos.z) / voxels::Chunk::CHUNK_DEPTH);
			ActorSystem::tick(m_registry, player.actor, dt, state.second);
			pos = m_registry->get<Position>(player.actor).position;
			const math::vec3i newPos(
			    static_cast<int>(pos.x) / voxels::Chunk::CHUNK_WIDTH,
			    static_cast<int>(pos.y) / voxels::Chunk::CHUNK_HEIGHT,
			    static_cast<int>(pos.z) / voxels::Chunk::CHUNK_DEPTH);
			if (!(oldPos == newPos))
			{
				for (const auto& chunk :
				     PlayerView::update(m_registry, player.actor, &m_map))
				{
					m_iris->sendData(player.id, chunk);
				}
			}
		}

		// Process events second
		size_t size = m_iris->eventQueue.size();
		for (size_t i = 0; i < size; i++)
		{
			net::Event event = m_iris->eventQueue.pop();
			;
			switch (event.type)
			{
			case net::Event::Type::CONNECT:
			{
				auto entity = m_registry->get<Player>(event.player);
				for (const auto& chunk :
				     PlayerView::update(m_registry, entity.actor, &m_map))
				{
					m_iris->sendData(entity.id, chunk);
				}
				break;
			}
			default:
				LOG_WARNING("GAME") << "Event received with unknown type";
			}
		}

		// Process messages last
		size = m_iris->messageQueue.size();
		for (size_t i = 0; i < size; i++)
		{
			net::MessageBundle message = m_iris->messageQueue.front();
			m_commander->run(message.userID, message.message);
			m_iris->messageQueue.pop();
		}

		// Dispatch confirmation states
		m_iris->sendState(m_registry, m_currentState.sequence);
	}
}