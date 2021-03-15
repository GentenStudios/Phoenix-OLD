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

#include <Common/Math/Vector3.hpp>

namespace phx::math::detail
{
	/**
	 * @brief A 4x4 Matrix, used for 3D graphics calculations.
	 *
	 * For more information on Matrices, a simple google search will
	 * reveal it's secrets! It's a very common topic on both game
	 * development tutorials AND general mathematics.
	 */
	class Matrix4x4
	{
		// workaround to prevent circular includes of Math.hpp.
		using Vec3 = detail::Vector3<float>;

	public:
		Matrix4x4();

		// this is just because the matrix is 4x4 and it's so much nicer
		// to read like this...
		// clang-format off
        Matrix4x4(float m00, float m10, float m20, float m30,
                 float m01, float m11, float m21, float m31,
                 float m02, float m12, float m22, float m32,
                 float m03, float m13, float m23, float m33);
		// clang-format on

		~Matrix4x4() = default;

		float elements[16];

		/**
		 * @brief Sets the Matrix to an Identity matrix.
		 *
		 * A simple google will explain matrix maths if you don't know
		 * any. :)
		 */
		void setIdentity();

		/**
		 * @brief Constructs a Perspective matrix for use in shaders.
		 * @param aspectRatio The aspect ratio of the viewport.
		 * @param fieldOfView The field of view the user experiences.
		 * @param farPlane The furthest that gets rendered.
		 * @param nearPlane The closest thing that gets rendered.
		 * @return The calculated matrix.
		 *
		 * A simple search will show what a perspective matrix is.
		 */
		static Matrix4x4 perspective(const float& aspectRatio,
		                             const float& fieldOfView,
		                             const float& farPlane,
		                             const float& nearPlane);

		/**
		 * @brief
		 * @param left
		 * @param right
		 * @param top
		 * @param bottom
		 * @param farPlane
		 * @param nearPlane
		 * @return
		 *
		 * @todo Fill in this, need to re-read the maths on this.
		 */
		static Matrix4x4 ortho(float left, float right, float top, float bottom,
		                       float farPlane, float nearPlane);

		/**
		 * @brief Constructs a View matrix for use in the shader.
		 * @param eyePos The position of the eye, or the user.
		 * @param centre The position that is being looked at.
		 * @param up The upwards direction.
		 * @return The calculated view matrix.
		 *
		 * A simple search will show what a view matrix is.
		 */
		static Matrix4x4 lookAt(const Vec3& eyePos, const Vec3& centre,
		                        const Vec3& up);

		// operator overloads.
		void operator*=(const float& other);
		void operator*=(const Matrix4x4& other);

		Matrix4x4 operator*(const float& other);
		Matrix4x4 operator*(const Matrix4x4& other) const;
		Vec3      operator*(const Vec3& other);
	};
} // namespace phx::math::detail

