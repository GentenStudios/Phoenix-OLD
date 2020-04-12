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

#include <Client/InputMap.hpp>
#include <Client/InputQueue.hpp>

using namespace phx::client;
using namespace phx;

InputQueue::InputQueue()
{
	m_forward  = InputMap::get()->getInput("core.move.forward");
	m_backward = InputMap::get()->getInput("core.move.backward");
	m_left     = InputMap::get()->getInput("core.move.left");
	m_right    = InputMap::get()->getInput("core.move.right");
	m_up       = InputMap::get()->getInput("core.move.up");
	m_down     = InputMap::get()->getInput("core.move.down");
}

void InputQueue::run(float dt)
{
	m_running = true;
	while (m_running)
	{
		InputState input;
		if (InputMap::get()->getState(m_forward))
		{
			input.forward = true;
		}
		if (InputMap::get()->getState(m_backward))
		{
			input.backward = true;
		}
		if (InputMap::get()->getState(m_left))
		{
			input.left = true;
		}
		if (InputMap::get()->getState(m_right))
		{
			input.right = true;
		}
		if (InputMap::get()->getState(m_up))
		{
			input.up = true;
		}
		if (InputMap::get()->getState(m_down))
		{
			input.down = true;
		}
		input.sequence = currentSequence() + 1;
		m_queue.push_back(input);
	}
}

void InputQueue::kill() { m_running = false; }

InputState InputQueue::getState(std::size_t sequence)
{
	for (const auto& state : m_queue)
	{
		if (state.sequence == sequence)
		{
			return state;
		}
	}
}

void InputQueue::clearState(std::size_t sequence)
{
	while (m_queue.front().sequence <= sequence)
	{
		m_queue.pop_front();
	}
}

std::size_t InputQueue::currentSequence() { return m_queue.back().sequence; }