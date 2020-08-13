// Copyright 2020 Genten Studios
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

#include <Common/Movement.hpp>
#include <Common/PlayerView.hpp>
#include <Common/Position.hpp>

using namespace phx;

voxels::BlockReferrer* ActorSystem::m_blockReferrer = nullptr;

void ActorSystem::setBlockReferrer(voxels::BlockReferrer* referrer)
{
	m_blockReferrer = referrer;
}

entt::entity ActorSystem::registerActor(entt::registry* registry)
{
	auto entity = registry->create();
	registry->emplace<Position>(entity, math::vec3 {0, 0, 0},
	                            math::vec3 {0, 0, 0});
	registry->emplace<Movement>(entity, DEFAULT_MOVE_SPEED);
	return entity;
}
void ActorSystem::tick(entt::registry* registry, entt::entity entity,
                       const float dt, const InputState& input)
{
	auto& pos = registry->get<Position>(entity);

	/// conversion from 1/1000 of degrees to rad
	pos.rotation.x = static_cast<float>(input.rotation.x) / 360000.0;
	pos.rotation.y = static_cast<float>(input.rotation.y) / 360000.0;
	const auto moveSpeed =
	    static_cast<float>(registry->get<Movement>(entity).moveSpeed);

	if (input.forward)
	{
		pos.position += pos.getForward() * dt * moveSpeed;
	}
	else if (input.backward)
	{
		pos.position -= pos.getForward() * dt * moveSpeed;
	}

	if (input.left)
	{
		pos.position -= pos.getRight() * dt * moveSpeed;
	}
	else if (input.right)
	{
		pos.position += pos.getRight() * dt * moveSpeed;
	}

	if (input.up)
	{
		pos.position.y += dt * moveSpeed;
	}
	else if (input.down)
	{
		pos.position.y -= dt * moveSpeed;
	}
}

static const float RAY_INCREMENT = 0.5f;

math::Ray ActorSystem::getTarget(entt::registry* registry, entt::entity entity)
{
	math::vec3   pos = (registry->get<Position>(entity).position / 2.f) + .5f;
	voxels::Map* map = registry->get<PlayerView>(entity).map;

	math::Ray ray(pos, registry->get<Position>(entity).getDirection());

	while (ray.getLength() < m_reach)
	{
		pos.floor();

		if (map->getBlockAt(pos)->category == voxels::BlockCategory::SOLID)
		{
			return ray;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return ray;
}

bool ActorSystem::action1(entt::registry* registry, entt::entity entity)
{
	math::vec3   pos = (registry->get<Position>(entity).position / 2.f) + .5f;
	voxels::Map* map = registry->get<PlayerView>(entity).map;

	math::Ray ray(pos, registry->get<Position>(entity).getDirection());

	while (ray.getLength() < m_reach)
	{
		pos.floor();

		const auto* currentBlock = map->getBlockAt(pos);
		if (currentBlock->category == voxels::BlockCategory::SOLID)
		{
			map->setBlockAt(
			    pos, m_blockReferrer->blocks.get(voxels::BlockType::AIR_BLOCK));

			if (currentBlock->onBreak)
			{
				currentBlock->onBreak(pos.x, pos.y, pos.z);
			}

			return true;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return false;
}

bool ActorSystem::action2(entt::registry* registry, entt::entity entity)
{
	math::vec3   pos = (registry->get<Position>(entity).position / 2.f) + .5f;
	voxels::Map* map = registry->get<PlayerView>(entity).map;

	math::Ray ray(pos, registry->get<Position>(entity).getDirection());

	while (ray.getLength() < m_reach)
	{
		pos.floor();

		const auto currentBlock = map->getBlockAt(pos);
		if (currentBlock->category == voxels::BlockCategory::SOLID)
		{
			math::vec3 back = ray.backtrace(RAY_INCREMENT);
			back.floor();

			map->setBlockAt(back, registry->get<Hand>(entity).hand);

			if (registry->get<Hand>(entity).hand->onPlace)
			{
				registry->get<Hand>(entity).hand->onPlace(back.x, back.y,
				                                          back.z);
			}

			return true;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return false;
}