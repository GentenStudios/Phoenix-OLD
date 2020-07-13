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

#include <Common/Actor.hpp>
#include <Common/Movement.hpp>
#include <Common/PlayerView.hpp>

using namespace phx;

Player::Player(entt::registry* registry) : m_registry(registry)
{
	m_entity = m_registry->create();
	m_registry->emplace<Position>(m_entity, math::vec3 {0, 0, 0},
	                              math::vec3 {0, 0, 0});
	m_registry->emplace<Movement>(m_entity, DEFAULT_MOVE_SPEED);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	std::vector<gfx::ShaderLayout> layout;
	layout.emplace_back("position", 0);
	m_pipeline.prepare("Assets/SimpleLines.vert", "Assets/SimpleLines.frag",
	                   layout);
}

void Player::registerAPI(cms::ModManager* manager)
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

void Player::renderSelectionBox(const math::mat4 view, const math::mat4 proj)
{
	auto pos =
	    ActorSystem::getTarget(m_registry, m_entity).getCurrentPosition();
	pos.floor();
	// do not waste cpu time if we aren't targeting a solid block
	if (m_registry->get<PlayerView>(m_entity).map->getBlockAt(pos)->category !=
	    voxels::BlockCategory::SOLID)
		return;

	// voxel position to camera position
	pos.x = (pos.x - 0.5f) * 2.f;
	pos.y = (pos.y - 0.5f) * 2.f;
	pos.z = (pos.z - 0.5f) * 2.f;

	/*
	       1 +--------+ 2
	        /|       /|
	       / |   3  / |
	    0 +--------+  |
	      |  |6    |  |
	      |  x-----|--+ 7
	      | /      | /
	      |/       |/
	    5 +--------+ 4
	 */

	const float more = 2.001f;
	const float less = 0.001f;

	float vertices[] = {pos.x + more, pos.y + more, pos.z - less, // 0-1
	                    pos.x - less, pos.y + more, pos.z - less,

	                    pos.x - less, pos.y + more, pos.z - less, // 1-2
	                    pos.x - less, pos.y + more, pos.z + more,

	                    pos.x - less, pos.y + more, pos.z + more, // 2-3
	                    pos.x + more, pos.y + more, pos.z + more,

	                    pos.x + more, pos.y + more, pos.z + more, // 3-4
	                    pos.x + more, pos.y - less, pos.z + more,

	                    pos.x + more, pos.y - less, pos.z + more, // 4-5
	                    pos.x + more, pos.y - less, pos.z - less,

	                    pos.x + more, pos.y - less, pos.z - less, // 5-6
	                    pos.x - less, pos.y - less, pos.z - less,

	                    pos.x - less, pos.y - less, pos.z - less, // 6-7
	                    pos.x - less, pos.y - less, pos.z + more,

	                    pos.x - less, pos.y - less, pos.z + more, // 7-4
	                    pos.x + more, pos.y - less, pos.z + more,

	                    pos.x - less, pos.y - less, pos.z + more, // 7-2
	                    pos.x - less, pos.y + more, pos.z + more,

	                    pos.x - less, pos.y + more, pos.z - less, // 1-6
	                    pos.x - less, pos.y - less, pos.z - less,

	                    pos.x + more, pos.y + more, pos.z - less, // 0-3
	                    pos.x + more, pos.y + more, pos.z + more,

	                    pos.x + more, pos.y + more, pos.z - less, // 0-5
	                    pos.x + more, pos.y - less, pos.z - less};

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	m_pipeline.activate();
	m_pipeline.setMatrix("u_view", view);
	m_pipeline.setMatrix("u_projection", proj);
	glDrawArrays(GL_LINES, 0, 24);
}