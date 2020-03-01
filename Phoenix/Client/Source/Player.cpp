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

#include <Client/Player.hpp>

#include <Common/Voxels/BlockRegistry.hpp>
#include <Common/ContentLoader.hpp>
#include <Common/Commander.hpp>

using namespace phx;

static const float RAY_INCREMENT = 0.5f;

Player::Player(voxels::ChunkView* world) : m_world(world) 
{
	ContentManager::get()->lua["core"]["player"] = 
		/**
		 * @addtogroup luaapi
		 *
		 * ---
		 * @subsection coreplayer core.player
		 * @brief Interfaces with the player
		 */
		ContentManager::get()->lua.create_table();
	ContentManager::get()->lua["core"]["player"]["getSpeed"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreplayergetspeed core.player.getSpeed()
	     * @brief Gets the players speed
		 * 
		 * @return The players speed
	     */
	    [this]() {
		    return getMoveSpeed();
	    };
	ContentManager::get()->lua["core"]["player"]["setSpeed"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreplayersetspeed core.player.setSpeed(speed)
	     * @brief Sets the player speed
	     *
	     * @param key The new speed to set the player to
	     *
	     */
	    [this](int speed) {
		   setMoveSpeed(speed);
	    };
	ContentManager::get()->lua["core"]["player"]["getPosition"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreplayergetposition core.player.getPosition()
	     * @brief Gets the players position
		 * 
		 * @return The players position in a table
		 * 
		 * @par Example usage
		 * 
		 * @code
		 * print(core.player.getPosition().x)
		 * print(core.player.getPosition().y)
		 * print(core.player.getPosition().z)
		 * @endcode
	     */
	    [this]() {
			sol::table pos = ContentManager::get()->lua.create_table();
			pos["x"] = getPosition().x;
			pos["y"] = getPosition().y;
			pos["z"] = getPosition().z;
		    return pos;
	    };
	ContentManager::get()->lua["core"]["player"]["setPosition"] =
	    /**
	     * @addtogroup luaapi
	     *
	     * @subsubsection coreplayersetposition core.player.setPosition(position)
	     * @brief Sets the player's position
	     *
	     * @param posx The x component of the players position
	     * @param posy The y component of the players position
	     * @param posz The z component of the players position
	     *
	     */
	    [this](int posx, int posy, int posz) {
		   setPosition({posx, posy, posz});
	    };

	CommandBook::get()->add(
		"tp", 
		"Teleports player to supplied coordiantes \n /tp <x> <y> <z>", 
		"all", 
		[this](const std::vector<std::string>& args){
			setPosition({std::stoi(args[0]), std::stoi(args[1]), std::stoi(args[2])});
		});
}

math::Ray Player::getTarget() const
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
			    back, m_hand);

			return true;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return false;
}

void Player::setHand(voxels::BlockType* block)
{
	m_hand = block;
}

voxels::BlockType* Player::getHand()
{
	return m_hand;
}
