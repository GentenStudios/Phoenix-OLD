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

#include <Quartz2/Camera.hpp>
#include <Quartz2/MathUtils.hpp>

using namespace q2;

Camera::Camera()
	: m_enabled(true)
{
}

Mat4 Camera::calculateViewMatrix()
{
	const Vec3 centre = m_position + m_direction;

	return Mat4::lookAt(m_position, centre, m_up);
}

bool Camera::isEnabled() const
{
	return m_enabled;
}

void Camera::update(float dt, SDL_Window* window)
{
	printf("%f\n", dt);
	const float SENSITIVITY = 0.00005f;

	if (!m_enabled)
		return;

	int windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	const int halfWindowWidth = windowWidth / 2;
	const int halfWindowHeight = windowHeight / 2;

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	SDL_WarpMouseInWindow(window, halfWindowWidth, halfWindowHeight);

	int dtX = (mouseX - halfWindowWidth);
	int dtY = (mouseY - halfWindowHeight);

	m_rotation.x += SENSITIVITY * dt * dtX;
	m_rotation.y += SENSITIVITY * dt * dtY;

	if (m_rotation.y > q2::PIDIV2)
		m_rotation.y = q2::PIDIV2;
	else if (m_rotation.y < -q2::PIDIV2)
		m_rotation.y = -q2::PIDIV2;

	m_direction.x = std::cosf(m_rotation.y) * std::sinf(m_rotation.x);
	m_direction.y = std::sinf(m_rotation.y);
	m_direction.z = std::cosf(m_rotation.y) * std::cosf(m_rotation.x);

	const Vec3 right = Vec3(
		std::sinf(m_rotation.x - q2::PIDIV2),
		0.f,
		std::cosf(m_rotation.x - q2::PIDIV2)
	);

	m_up = Vec3::cross(right, m_direction);

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	static const float BASE_MOVE_SPEED = 0.1f;
	
	if (keys[SDL_SCANCODE_W])
		m_position += m_direction * dt * BASE_MOVE_SPEED;
	else if (keys[SDL_SCANCODE_S])
		m_position -= m_direction * dt * BASE_MOVE_SPEED;

	if (keys[SDL_SCANCODE_A])
		m_position -= right * dt * BASE_MOVE_SPEED;
	else if (keys[SDL_SCANCODE_D])
		m_position += right * dt * BASE_MOVE_SPEED;
}

void Camera::setPosition(Vec3 newPosition)
{
	m_position = newPosition;
}

void Camera::toggleEnabled()
{
	m_enabled = !m_enabled;
}