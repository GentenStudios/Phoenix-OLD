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

#pragma once

#include <Common/Math/Math.hpp>
#include <Common/Voxels/Map.hpp>

namespace phx
{
	/**
	 * @brief The positioning for an entity
	 */
	struct Position
	{
		/// @brief The direction the entity is facing
		math::vec3 rotation;
		/// @brief The cardinal position of the entity
		math::vec3 position;
		/// @brief The map the entity is located on
        voxels::Map* map;

		/**
		 * @brief Gets the direction the object is facing.
		 * @return The 3-component vector representing direction.
		 *
		 * The direction is always the direction the player is looking, this
		 * can be used with the Ray object to find what the player is
		 * actually looking at within the actual world, whether it be voxels
		 * or something else. Because this is not representative of
		 * position, and direction cannot be skewed by the world coordinate
		 * system, this data is compatible with every other layer requiring
		 * and supporting direction, of course when asking for a 3
		 * dimensional vector.
		 */
		math::vec3 getDirection() const
		{
			return math::vec3 {std::cos(rotation.y) * std::sin(rotation.x),
			                   std::sin(rotation.y),
			                   std::cos(rotation.y) * std::cos(rotation.x)};
		};

		math::vec3 getRight() const
		{
			return math::vec3 {std::sin(rotation.x - math::PIDIV2), 0.f,
			                   std::cos(rotation.x - math::PIDIV2)};
		};

		math::vec3 getForward() const
		{
			return math::vec3 {std::sin(rotation.x), 0.f, std::cos(rotation.x)};
		};

		math::vec3 getUp() const
        {
			return math::vec3::cross(getRight(), getDirection());
		};

		math::mat4 getView() const
        {
			return math::mat4::lookAt(position, (position + getDirection()), getUp());
		};
	};
} // namespace phx
