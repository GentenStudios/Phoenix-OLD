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

#include <Quartz2/Camera.hpp>
#include <Quartz2/Math/MathUtils.hpp>

#include <cmath>

const float MOVE_SPEED  = 0.01f;
const float SENSITIVITY = 0.00005f;

using namespace q2;

Camera::Camera()
{
	m_settingSensitivity =
	    Settings::get()->add("Sensitivity", "camera:sensitivity", 5);
	m_settingSensitivity->setMax(100);
	m_settingSensitivity->setMin(1);
}

math::vec3 Camera::getPosition() const { return m_position; }

math::vec3 Camera::getDirection() const { return m_direction; }

math::mat4 Camera::calculateViewMatrix() const
{
	const math::vec3 centre = m_position + m_direction;
	return math::mat4::lookAt(m_position, centre, m_up);
}

void Camera::enable(bool enabled) { m_enabled = enabled; }

void Camera::tick(float dt, SDL_Window* window)
{
	if (!m_enabled)
		return;

	int windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	const int halfWindowWidth  = windowWidth / 2;
	const int halfWindowHeight = windowHeight / 2;

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	SDL_WarpMouseInWindow(window, halfWindowWidth, halfWindowHeight);

	const float sensitivity = 0.00001 * m_settingSensitivity->value();

	m_rotation.x += sensitivity * dt * (halfWindowWidth - mouseX);
	m_rotation.y += sensitivity * dt * (halfWindowHeight - mouseY);

	m_rotation.y = math::clamp(m_rotation.y, -math::PIDIV2, math::PIDIV2);

	m_direction.x = std::cos(m_rotation.y) * std::sin(m_rotation.x);
	m_direction.y = std::sin(m_rotation.y);
	m_direction.z = std::cos(m_rotation.y) * std::cos(m_rotation.x);

	const math::vec3 right = {std::sin(m_rotation.x - math::PIDIV2), 0.f,
	                    std::cos(m_rotation.x - math::PIDIV2)};

	m_up = math::vec3::cross(right, m_direction);

	const float moveSpeed = MOVE_SPEED;

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_W])
	{
		m_position += m_direction * dt * moveSpeed;
	}
	else if (keys[SDL_SCANCODE_S])
	{
		m_position -= m_direction * dt * moveSpeed;
	}

	if (keys[SDL_SCANCODE_A])
	{
		m_position -= right * dt * moveSpeed;
	}
	else if (keys[SDL_SCANCODE_D])
	{
		m_position += right * dt * moveSpeed;
	}

	if (keys[SDL_SCANCODE_SPACE])
	{
		m_position.y += dt * moveSpeed;
	}
	else if (keys[SDL_SCANCODE_LSHIFT])
	{
		m_position.y -= dt * moveSpeed;
	}

	if (keys[SDL_SCANCODE_P])
	{
		m_settingSensitivity->set(m_settingSensitivity->value() +
		                                       1);
	}
	else if (keys[SDL_SCANCODE_O])
	{
		m_settingSensitivity->set(m_settingSensitivity->value() -
		                                       1);
	}
}