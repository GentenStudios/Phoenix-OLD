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

#include <Client/InputQueue.hpp>

#include <Common/Position.hpp>

using namespace phx::client;
using namespace phx;

InputQueue::InputQueue(entt::registry* registry, Player* player,
                       gfx::FPSCamera* camera)
    : m_player(player), m_registry(registry), m_camera(camera),
      m_forward(client::InputMap::get()->registerInput(
          "core.move.forward", "Move Forward", events::Keys::KEY_W)),
      m_backward(client::InputMap::get()->registerInput(
          "core.move.backward", "Move Backward", events::Keys::KEY_S)),
      m_left(client::InputMap::get()->registerInput(
          "core.move.left", "Strafe Left", events::Keys::KEY_A)),
      m_right(client::InputMap::get()->registerInput(
          "core.move.right", "Strafe Right", events::Keys::KEY_D)),
      m_up(client::InputMap::get()->registerInput("core.move.up", "Fly Up",
                                                  events::Keys::KEY_SPACE)),
      m_down(client::InputMap::get()->registerInput(
          "core.move.down", "Fly Down", events::Keys::KEY_LEFT_SHIFT))
{
}

InputQueue::~InputQueue()
{
	if (m_running)
	{
		stop();
	}
}

void InputQueue::run(std::chrono::milliseconds dt, client::Network* network)
{
	using std::chrono::system_clock;
	system_clock::time_point next = system_clock::now();
	while (m_running)
	{
		m_sequence++;
		InputState state = getCurrentState();
		m_queue.push(state);
		network->sendState(state);
		next = next + dt;
		std::this_thread::sleep_until(next);
	}
}

void InputQueue::start(std::chrono::milliseconds dt, client::Network* network)
{
	m_running           = true;
	std::thread thread1 = std::thread(&InputQueue::run, this, dt, network);
	std::swap(m_thread, thread1);
}

void InputQueue::stop()
{
	m_running = false;
	m_thread.join();
}

InputState InputQueue::getCurrentState()
{
	InputState input;
	if (m_camera->isEnabled())
	{
		input.forward  = InputMap::get()->getState(m_forward);
		input.backward = InputMap::get()->getState(m_backward);
		input.left     = InputMap::get()->getState(m_left);
		input.right    = InputMap::get()->getState(m_right);
		input.up       = InputMap::get()->getState(m_up);
		input.down     = InputMap::get()->getState(m_down);
	}
	input.rotation.x = static_cast<int>(
	    m_registry->get<Position>(m_player->getEntity()).rotation.x * 360000.0);
	input.rotation.y = static_cast<int>(
	    m_registry->get<Position>(m_player->getEntity()).rotation.y * 360000.0);
	input.sequence = m_sequence;
	return input;
}
