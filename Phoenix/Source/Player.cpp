// Copyright 2019 Genten Studios
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

#include <Phoenix/Math/Math.hpp>
#include <Phoenix/Player.hpp>
#include <Phoenix/Voxels/Block.hpp>
#include <Phoenix/Voxels/BlockRegistry.hpp>

using namespace phx;

static const float RAY_INCREMENT = 0.5f;

math::Ray Player::getTarget()
{

	math::vec3 pos = (getPosition() / 2.f) + .5f;

	math::Ray ray(pos, getDirection());

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
	math::vec3 pos = (getPosition() / 2.f) + .5f;

	math::Ray ray(pos, getDirection());

	while (ray.getLength() < m_reach)
	{
		pos.floor();

		if (m_world->getBlockAt(pos)->category == voxels::BlockCategory::SOLID)
		{
			m_world->setBlockAt(
			    pos, voxels::BlockRegistry::get()->getFromID("core:air"));

			return true;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return false;
}

bool Player::action2()
{
	math::vec3 pos = (getPosition() / 2.f) + .5f;

	math::Ray ray(pos, getDirection());

	while (ray.getLength() < m_reach)
	{
		pos.floor();

		if (m_world->getBlockAt(pos)->category == voxels::BlockCategory::SOLID)
		{
			math::vec3 back = ray.backtrace(RAY_INCREMENT);
			back.floor();

			m_world->setBlockAt(
			    back, voxels::BlockRegistry::get()->getFromID("core:dirt"));

			return true;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return false;
}
