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

#pragma once

#include <algorithm>

namespace q2
{
	static constexpr float PI = 3.141592654f;
	static constexpr float PIDIV2 = 1.570796327f;
	static constexpr float PIDIV4 = 0.785398163f;

	/**
	 * @brief Converts degrees to radians.
	 * @param degrees The number of degrees waiting to be converted.
	 * @return The converted value, in radians.
	 */
	static float degreeToRadians(const float& degrees)
	{
		return degrees * PI / 180.f;
	}

	/**
	 * @brief Converts radians to degrees.
	 * @param radians The number of radians waiting to be converted.
	 * @return The converted value, in degrees.
	 */
	static float radianToDegrees(const float& radians)
	{
		return radians * 180.f / PI;
	}

	/**
	 * @brief
	 * @tparam T The data type that needs to be clamped.
	 * @param n The actual value needing to be clamped.
	 * @param lower The lowest value the result should be allowed to be.
	 * @param upper The highest value the results should be allowed to be.
	 * @return Either the original number if the value is between the lower
	 *         and upper bounds, OR the upper/lower bounds dependant
	 *         on the value.
	 */
	template <typename T>
	T clamp(const T& n, const T& lower, const T& upper)
	{
		return std::max(lower, std::min(n, upper));
	}
} // namespace q2
