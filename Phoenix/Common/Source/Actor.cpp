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

#include <Common/Actor.hpp>

using namespace phx;

static const int MAX_MOVE_SPEED     = 500;
static const int DEFAULT_MOVE_SPEED = 10;

Actor::Actor() : m_moveSpeed(DEFAULT_MOVE_SPEED) {}

math::vec3 Actor::getPosition() const { return m_position; }

bool Actor::setPosition(math::vec3 pos)
{
	m_position = pos;
	return true;
}

math::vec3 Actor::getRotation() const { return m_rotation; }

bool Actor::setRotation(math::vec3 rot)
{
	m_rotation = rot;
	return true;
}

math::vec3 Actor::getDirection() const
{
	return {std::cos(m_rotation.y) * std::sin(m_rotation.x),
	        std::sin(m_rotation.y),
	        std::cos(m_rotation.y) * std::cos(m_rotation.x)};
}

int Actor::getMoveSpeed() const { return m_moveSpeed; }

bool Actor::setMoveSpeed(int speed)
{
	if (speed >= 0 && speed <= MAX_MOVE_SPEED)
	{
		m_moveSpeed = speed;
		return true;
	}

	return false;
}

