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
#include <Common/Movement.hpp>
#include <Common/Position.hpp>

using namespace phx;
using namespace phx::server;

Game::Game(entt::registry* registry, bool* running, networking::Iris* iris)
    : m_registry(registry), m_running(running), m_iris(iris)
{
}

void Game::run()
{
	static const float dt = 1.f / 20.f;

	while (m_running)
	{
		// @todo @beeper is this efficient? we process a state 20 times a second
		while (m_iris->stateQueue.front().ready == false)
		{ /* Waiting */
		}

		networking::StateBundle m_currentState = m_iris->stateQueue.front();
		m_iris->stateQueue.pop_front();

		for (auto state : m_currentState.states)
		{
			ActorSystem::tick(m_registry,
			                  m_registry->get<Player>(*state.first).actor, dt,
			                  state.second);
			std::cout << m_registry
			                 ->get<Position>(
			                     m_registry->get<Player>(*state.first).actor)
			                 .position
			          << "\n";
		}
	}
}