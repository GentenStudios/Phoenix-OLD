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

#include <Client/Graphics/Camera.hpp>

#include <Common/Position.hpp>

using namespace phx;
using namespace gfx;

FPSCamera::FPSCamera(Window* window, entt::registry* registry)
    : m_registry(registry), m_enabled(true), m_window(window)
{
	m_window->setCursorState(CursorState::DISABLED);

	// Calculates the perspective projection
	const math::vec2 windowSize = window->getSize();
	m_projection = math::mat4::perspective(windowSize.x / windowSize.y, 45.f,
	                                       1000.f, 0.1f);

	// This stores the centre of the window to prevent requesting the window
	// size every frame, which is incredibly inefficient.
	m_windowCentre = {std::floor(windowSize.x / 2.f),
	                  std::floor(windowSize.y / 2.f)};

	m_settingSensitivity =
	    Settings::get()->add("Sensitivity", "camera:sensitivity", 50);
	m_settingSensitivity->setMax(100);
	m_settingSensitivity->setMin(1);
}

void FPSCamera::setProjection(const math::mat4& projection)
{
	m_projection = projection;
}

math::mat4 FPSCamera::getProjection() const { return m_projection; }

math::mat4 FPSCamera::calculateViewMatrix() const
{
	const Position&  pos    = m_registry->get<Position>(m_actor);
	const math::vec3 centre = pos.position + pos.getDirection();
	return math::mat4::lookAt(pos.position, centre, m_up);
}

void FPSCamera::tick(float dt)
{
	// makes sure we don't waste CPU cycles if we've disabled the camera.
	if (!m_enabled)
		return;

	// make sure we don't segfault because we haven't set the actor yet.
	if (!m_registry->valid(m_actor))
		return;

	const math::vec2 mousePos = m_window->getCursorPosition();

	m_window->setCursorPosition(m_windowCentre);

	const float sensitivity =
	    static_cast<float>(m_settingSensitivity->value()) / 50.f;

	Position& pos = m_registry->get<Position>(m_actor);

	/// @todo Fix this up since we're having an issue where we turn more/less
	/// due to higher/lower frames.
	pos.rotation.x += sensitivity * dt * (m_windowCentre.x - mousePos.x);
	pos.rotation.y += sensitivity * dt * (m_windowCentre.y - mousePos.y);

	pos.rotation.y = math::clamp(pos.rotation.y, -math::PIDIV2, math::PIDIV2);

	m_up = math::vec3::cross(pos.getRight(), pos.getDirection());

	/// @todo Change this behavior when we'll move the position management out of the camera (but Toby said it was fine for now)
	if (m_fly)
		return;

	math::vec3 position = pos.position;
	// camera position to voxel position
	position = (position / 2.f) + 0.5f;
	position.y -= 1;

	voxels::BlockType* blockType = m_world->getBlockAt(position);
	if (blockType->category != voxels::BlockCategory::SOLID)  // no collision, update the position
	{
		// check if we're grounded before updating
		/// @todo handles Archimedes' principle in LIQUIDs
		if (m_world->getBlockAt(math::vec3(position.x, position.y - 1, position.z))->category != voxels::BlockCategory::SOLID)
			///@todo make a constant for gravity
			position.y -= dt * 3.f;
		m_registry->get<Position>(m_actor).position = position;
	}
<<<<<<< HEAD
=======

	///@todo change this behavior when we'll move the position management out of the camera
	math::vec3 pos = m_position;
	// camera position to voxel position
	pos = (pos / 2.f) + 0.5f;

	voxels::BlockType* blockType = m_world->getBlockAt(pos);
	if (blockType->category != voxels::BlockCategory::SOLID)  // no collision, update the position
	{
		// check if we're grounded before updating
		///@todo handles Archimedes' principle in LIQUIDs
		if (m_world->getBlockAt(math::vec3(pos.x, pos.y - 1, pos.z))->category != voxels::BlockCategory::SOLID)
			///@todo make a constant for gravity
			m_position.y -= dt * 3.f;
		m_registry->get<Position>(m_actor).position = m_position;
	}
	m_registry->get<Position>(m_actor).rotation = m_rotation;
>>>>>>> 7c7b5f2d7bedaffa4bd06c29fc255c47ec5b36a7
}

void FPSCamera::enable(bool enabled)
{
	if (enabled)
	{
		// gain control of the cursor
		m_window->setCursorState(gfx::CursorState::DISABLED);
		m_window->setCursorPosition(m_windowCentre);
	}
	else
	{
		// release cursor for the user to do whatever they want.
		m_window->setCursorState(gfx::CursorState::NORMAL);
	}

	m_enabled = enabled;
}

void FPSCamera::onWindowResize(events::Event e)
{
	// recalculate window size again, to not be inefficient.
	const math::vec2 windowSize = m_window->getSize();
	m_projection = math::mat4::perspective(windowSize.x / windowSize.y,
	                                       45.f, 1000.f, 0.1f);

	m_windowCentre = {static_cast<float>(static_cast<int>(windowSize.x / 2)),
	                  static_cast<float>(static_cast<int>(windowSize.y / 2))};
}

void FPSCamera::setActor(entt::entity actor) { m_actor = actor;}
void FPSCamera::setWorld(voxels::ChunkView* world) { m_world = world; }
void FPSCamera::setFly(bool value) { m_fly = value; }

bool FPSCamera::fly() const { return m_fly; }
