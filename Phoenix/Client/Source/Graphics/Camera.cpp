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
#include <Common/Movement.hpp>

const float MOVE_SPEED = 0.01f;

using namespace phx;
using namespace gfx;

FPSCamera::FPSCamera(Window* window, entt::registry* registry) :
m_registry(registry)
{
	m_window = window;
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

	m_forward = client::InputMap::get()->registerInput(
	    "core.move.forward", "Move Forward", events::Keys::KEY_W);
	m_backward = client::InputMap::get()->registerInput(
	    "core.move.backward", "Move Backward", events::Keys::KEY_S);
	m_left = client::InputMap::get()->registerInput(
	    "core.move.left", "Strafe Left", events::Keys::KEY_A);
	m_right = client::InputMap::get()->registerInput(
	    "core.move.right", "Strafe Right", events::Keys::KEY_D);
	m_fly  = client::InputMap::get()->registerInput("core.move.up", "Fly Up",
                                                   events::Keys::KEY_SPACE);
	m_down = client::InputMap::get()->registerInput(
	    "core.move.down", "Fly Down", events::Keys::KEY_LEFT_SHIFT);
}

math::vec3 FPSCamera::getPosition() const { return m_position; }

math::vec3 FPSCamera::getDirection() const { return m_direction; }

void FPSCamera::setProjection(const math::mat4& projection)
{
	m_projection = projection;
}

math::mat4 FPSCamera::getProjection() const { return m_projection; }

math::mat4 FPSCamera::calculateViewMatrix() const
{
	const math::vec3 centre = m_position + m_direction;
	return math::mat4::lookAt(m_position, centre, m_up);
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

	const float sensitivity = static_cast<float>(m_settingSensitivity->value()) / 50;

	m_rotation = m_registry->get<Position>(m_actor).rotation;
	m_position = m_registry->get<Position>(m_actor).position;

	/// @todo Fix this up since we're having an issue where we turn more/less
	/// due to higher/lower frames.
	m_rotation.x += sensitivity * dt * (m_windowCentre.x - mousePos.x);
	m_rotation.y += sensitivity * dt * (m_windowCentre.y - mousePos.y);

	m_rotation.y = math::clamp(m_rotation.y, -math::PIDIV2, math::PIDIV2);

	m_direction.x = std::cos(m_rotation.y) * std::sin(m_rotation.x);
	m_direction.y = std::sin(m_rotation.y);
	m_direction.z = std::cos(m_rotation.y) * std::cos(m_rotation.x);

	const math::vec3 right   = {std::sin(m_rotation.x - math::PIDIV2), 0.f,
                              std::cos(m_rotation.x - math::PIDIV2)};
	const math::vec3 forward = {std::sin(m_rotation.x), 0.f,
	                            std::cos(m_rotation.x)};

	m_up = math::vec3::cross(right, m_direction);

	const float moveSpeed =
	    static_cast<float>(m_registry->get<Movement>(m_actor).moveSpeed);

	if (m_window->isKeyDown(m_forward->key))
	{
		m_position += forward * dt * moveSpeed;
	}
	else if (m_window->isKeyDown(m_backward->key))
	{
		m_position -= forward * dt * moveSpeed;
	}

	if (m_window->isKeyDown(m_left->key))
	{
		m_position -= right * dt * moveSpeed;
	}
	else if (m_window->isKeyDown(m_right->key))
	{
		m_position += right * dt * moveSpeed;
	}

	if (m_window->isKeyDown(m_fly->key))
	{
		m_position.y += dt * moveSpeed;
	}
	else if (m_window->isKeyDown(m_down->key))
	{
		m_position.y -= dt * moveSpeed;
	}

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

void FPSCamera::setWorld(voxels::ChunkView* world) { m_world = world;}