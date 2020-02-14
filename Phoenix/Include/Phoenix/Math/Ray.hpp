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

#include <Phoenix/Math/Vector3.hpp>

namespace phx
{
	namespace math
	{
	/**
	 * @brief Produces a castable ray for helping find things at
	 * positions/intervals along the ray.
	 */
	class Ray
	{
		using vec3 = detail::Vector3<float>;
	public:
		/**
		 * @brief Constructs a Ray object.
		 * @param start The position of the start of the ray.
		 * @param direction The direction the ray is "traveling" in.
		 */
		Ray(const vec3& start, const vec3& direction);

		Ray(const Ray& other) = default;
		~Ray()                = default;

		/**
		 * @brief Advances along a ray.
		 * @param scale The distance to advance along the ray
		 * @return The new position along the ray.
		 */
		vec3 advance(float scale);

		/**
		 * @brief Backtracks (goes backwards) along a ray.
		 * @param scale The distance to backtrack along the ray.
		 * @return The new position along the ray.
		 */
		vec3 backtrace(float scale);

		/**
		 * @brief Gets the current length of the ray.
		 * @return The length of the ray.
		 */
		float getLength() const;

		/**
		 * @brief Gets the current position along the ray.
		 * @return The current position along the ray
		 */
		vec3 getCurrentPosition() const;

	private:
		float      m_length;
		vec3 m_start;
		vec3 m_direction;
		vec3 m_currentPosition;
	};
	}
} // namespace phx
