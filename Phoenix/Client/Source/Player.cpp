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

#include <Common/Voxels/BlockRegistry.hpp>
#include <Common/ContentLoader.hpp>
#include <Common/Commander.hpp>

#include <Common/Movement.hpp>
#include <Common/Position.hpp>
#include <Common/Actor.hpp>

using namespace phx;

static const float RAY_INCREMENT = 0.5f;

Player::Player(voxels::ChunkView* world, entt::registry* registry)
    : m_world(world), m_registry(registry)
{
    m_entity = m_registry->create();
    m_registry->emplace<Position>( m_entity,
        math::vec3{0, 0, 0}, math::vec3{0, 0, 0});
    m_registry->emplace<Movement>(m_entity, DEFAULT_MOVE_SPEED);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	std::vector<gfx::ShaderLayout> layout;
	layout.emplace_back("position", 0);
	m_pipeline.prepare("Assets/SimpleLines.vert", "Assets/SimpleLines.frag", layout);

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
		    return m_registry->get<Movement>(m_entity).moveSpeed;
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
          m_registry->get<Movement>(m_entity).moveSpeed = speed;
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
			pos["x"] = m_registry->get<Position>(m_entity).position.x;
			pos["y"] = m_registry->get<Position>(m_entity).position.y;
			pos["z"] = m_registry->get<Position>(m_entity).position.z;
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
          m_registry->get<Position>(m_entity).position = {posx, posy, posz};
	    };

	CommandBook::get()->add("tp",
                            "Teleports player to supplied coordinates \n /tp <x> <y> <z>",
                            "all",
                            [this](const std::vector<std::string>& args){
                              m_registry->get<Position>(m_entity).position =
                                  {std::stoi(args[0]), std::stoi(args[1]), std::stoi(args[2])};
                            });
}

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

			m_world->setBlockAt(
			    back, m_registry->get<Hand>(getEntity()).hand);

			return true;
		}

		pos = ray.advance(RAY_INCREMENT);
	}

	return false;
}

math::vec3 Player::rotToDir(math::vec3 m_rotation){
    return {std::cos(m_rotation.y) * std::sin(m_rotation.x),
            std::sin(m_rotation.y),
            std::cos(m_rotation.y) * std::cos(m_rotation.x)};
}

void Player::renderSelectionBox(const math::mat4 view, const math::mat4 proj)
{
	auto pos = getTarget().getCurrentPosition();
	// do not waste cpu time if we aren't targetting a solid block
	if (m_world->getBlockAt(pos)->category != voxels::BlockCategory::SOLID)
		return;

	float vertices[] = {
		pos.x + 0.5f, pos.y + 0.5f, pos.z - 0.5f,
		pos.x - 0.5f, pos.y + 0.5f, pos.z - 0.5f,
		pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f,
		pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f,
		pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f,
		pos.x + 0.5f, pos.y - 0.5f, pos.z - 0.5f,
		pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f,
		pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f
	};

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	m_pipeline.activate();
	m_pipeline.setMatrix("u_view", view);
	m_pipeline.setMatrix("u_projection", proj);
	glDrawArrays(GL_LINES, 0, 8);
}