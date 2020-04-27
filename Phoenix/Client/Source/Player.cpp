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

#include <Client/Player.hpp>

#include <Common/Commander.hpp>
#include <Common/Voxels/BlockRegistry.hpp>

#include <Common/Actor.hpp>
#include <Common/Movement.hpp>
#include <Common/Position.hpp>

using namespace phx;

static const float RAY_INCREMENT = 0.5f;

Player::Player(entt::registry* registry)
    : m_registry(registry)
{
	m_entity = m_registry->create();
	m_registry->emplace<Position>(m_entity, math::vec3 {0, 0, 0},
	                              math::vec3 {0, 0, 0});
	m_registry->emplace<Movement>(m_entity, DEFAULT_MOVE_SPEED);

	CommandBook::get()->add(
	    "tp", "Teleports player to supplied coordinates \n /tp <x> <y> <z>",
	    "all", [this](const std::vector<std::string>& args) {
		    m_registry->get<Position>(m_entity).position = {
		        std::stoi(args[0]), std::stoi(args[1]), std::stoi(args[2])};
	    });
}

void Player::registerAPI(mods::ModManager* manager)
{
	manager->registerFunction("core.player.getSpeed", [this]() {
		return m_registry->get<Movement>(m_entity).moveSpeed;
	});

	manager->registerFunction("core.player.setSpeed", [this](int speed) {
		m_registry->get<Movement>(m_entity).moveSpeed = speed;
	});

	manager->registerFunction("core.player.getPosition", [this]() {
		sol::table pos;
		pos["x"] = m_registry->get<Position>(m_entity).position.x;
		pos["y"] = m_registry->get<Position>(m_entity).position.y;
		pos["z"] = m_registry->get<Position>(m_entity).position.z;
		return pos;
	});

	manager->registerFunction(
	    "core.player.setPosition", [this](int posx, int posy, int posz) {
		    m_registry->get<Position>(m_entity).position = {posx, posy, posz};
	    });
}

void Player::setWorld(voxels::ChunkView* world) { m_world = world; }

math::Ray Player::getTarget() const
{
	math::vec3 pos = (m_registry->get<Position>(m_entity).position / 2.f) + .5f;

	math::Ray ray(pos, rotToDir(m_registry->get<Position>(m_entity).rotation));

	while (ray.getLength() < m_reach)
	{
		pos.floor();

		if (m_world->getBlockAt(pos)->category == voxels::BlockCategory::SOLID)
		{
			return ray;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return ray;
}

bool Player::action1()
{
	math::vec3 pos = (m_registry->get<Position>(m_entity).position / 2.f) + .5f;

	math::Ray ray(pos, rotToDir(m_registry->get<Position>(m_entity).rotation));

	while (ray.getLength() < m_reach)
	{
		pos.floor();

		if (m_world->getBlockAt(pos)->category == voxels::BlockCategory::SOLID)
		{
			m_world->setBlockAt(
			    pos, voxels::BlockRegistry::get()->getFromID("core.air"));

			return true;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return false;
}

bool Player::action2()
{
	math::vec3 pos = (m_registry->get<Position>(m_entity).position / 2.f) + .5f;

	math::Ray ray(pos, rotToDir(m_registry->get<Position>(m_entity).rotation));

	while (ray.getLength() < m_reach)
	{
		pos.floor();

		if (m_world->getBlockAt(pos)->category == voxels::BlockCategory::SOLID)
		{
			math::vec3 back = ray.backtrace(RAY_INCREMENT);
			back.floor();

			m_world->setBlockAt(back, m_registry->get<Hand>(getEntity()).hand);

			return true;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return false;
}

math::vec3 Player::rotToDir(math::vec3 m_rotation)
{
	return {std::cos(m_rotation.y) * std::sin(m_rotation.x),
	        std::sin(m_rotation.y),
	        std::cos(m_rotation.y) * std::cos(m_rotation.x)};
}