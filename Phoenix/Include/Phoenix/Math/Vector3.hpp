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

#include <cmath>
#include <iostream>
#include <ostream>

namespace phx::math
{
	namespace detail
	{
		template <typename T>
		class Vector3
		{
		public:
			using ValueType           = T;
			using ValueReference      = T&;
			using ConstValueReference = const T&;

			static constexpr int SIZE = 3;
			static constexpr int size() { return SIZE; }

			union {
				T data[SIZE];
				struct
				{
					T x;
					T y;
					T z;
				};
				struct
				{
					T r;
					T g;
					T b;
				};
			};

			constexpr Vector3() : x(), y(), z() {}

			template <typename X>
			constexpr explicit Vector3(X val)
			    : x(static_cast<T>(val)), y(static_cast<T>(val)),
			      z(static_cast<T>(val))
			{
			}

			template <typename X, typename Y, typename Z>
			constexpr Vector3(X x, Y y, Z z)
			    : x(static_cast<T>(x)), y(static_cast<T>(y)),
			      z(static_cast<T>(z))
			{
			}

			template <typename X>
			explicit Vector3(const Vector3<X>& other)
			{
				x = static_cast<T>(other.x);
				y = static_cast<T>(other.y);
				z = static_cast<T>(other.z);
			}

			template <typename X>
			Vector3& operator=(const Vector3<X>& other)
			{
				x = static_cast<T>(other.x);
				y = static_cast<T>(other.y);
				z = static_cast<T>(other.z);

				return *this;
			}

			template <typename NewType>
			explicit constexpr operator Vector3<NewType>() const
			{
				Vector3<NewType> vec;

				vec.x = static_cast<NewType>(x);
				vec.y = static_cast<NewType>(y);
				vec.z = static_cast<NewType>(z);

				return vec;
			}

			/**
			 * @brief Create a Vector3 from a flattened cube number and cube
			 * side length
			 *
			 * @tparam X Flattened cube number
			 * @tparam Len Cube side length
			 */
			template <typename X, typename Len>
			constexpr explicit Vector3(X val, Len len)
			{
				z = static_cast<T>((val / (len * len)));
				y = static_cast<T>((val - (z * len * len)) / len);
				x = static_cast<T>((val - len * (y + (len * z))));
			}

			/**
			 * @brief Flattens a Vector3 representing a coordinate in a
			 * rectangle into a single value
			 *
			 * @param lenX Length of side X in rectangle
			 * @param lenY Length of side Y in rectangle
			 * @return constexpr Flattened position
			 */
			constexpr ValueType flatten(ValueType lenX, ValueType lenY)
			{
				return x + lenX * (y + lenY * z);
			}

			/**
			 * @brief Flattens a Vector3 representing a coordinate in a
			 * square into a single value
			 *
			 * @param len Length of a side in the square
			 * @return constexpr Flattened position
			 */
			constexpr ValueType flatten(ValueType len)
			{
				return x + len * (y + len * z);
			}

			void floor()
			{
				x = std::floor(x);
				y = std::floor(y);
				z = std::floor(z);
			}
			void ceil()
			{
				x = std::ceil(x);
				y = std::ceil(y);
				z = std::ceil(z);
			}

			void normalize()
			{
				const float magnitude = std::sqrt(dotProduct(*this, *this));

				x /= magnitude;
				y /= magnitude;
				z /= magnitude;
			}

			static Vector3 normalize(const Vector3& vec)
			{
				const float magnitude = std::sqrt(dotProduct(vec, vec));

				return Vector3(vec.x / magnitude, vec.y / magnitude,
				               vec.z / magnitude);
			}

			static Vector3 cross(const Vector3& vec1, const Vector3& vec2)
			{
				return Vector3(vec1.y * vec2.z - vec1.z * vec2.y,
				               vec1.z * vec2.x - vec1.x * vec2.z,
				               vec1.x * vec2.y - vec1.y * vec2.x);
			}

			static float dotProduct(const Vector3& vec1, const Vector3& vec2)
			{
				return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
			}

			bool operator==(const Vector3& rhs) const
			{
				return (x == rhs.x && y == rhs.y && z == rhs.z);
			}
			bool operator!=(const Vector3& rhs) const
			{
				return (x != rhs.x && y != rhs.y && z != rhs.z);
			}
			bool operator>(const Vector3& rhs) const
			{
				return (x > rhs.x && y > rhs.y && z > rhs.z);
			}
			bool operator<(const Vector3& rhs) const
			{
				return (x < rhs.x && y < rhs.y && z < rhs.z);
			}
			bool operator>=(const Vector3& rhs) const
			{
				return (x >= rhs.x && y >= rhs.y && z >= rhs.z);
			}
			bool operator<=(const Vector3& rhs) const
			{
				return (x <= rhs.x && y <= rhs.y && z <= rhs.z);
			}

			Vector3 operator+(const Vector3& rhs) const
			{
				return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
			}
			Vector3 operator-(const Vector3& rhs) const
			{
				return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
			}
			Vector3 operator*(const Vector3& rhs) const
			{
				return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
			}
			Vector3 operator/(const Vector3& rhs) const
			{
				return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
			}
			void operator+=(const Vector3& rhs)
			{
				x += rhs.x, y += rhs.y;
				z += rhs.z;
			}
			void operator-=(const Vector3& rhs)
			{
				x -= rhs.x, y -= rhs.y;
				z -= rhs.z;
			}
			void operator*=(const Vector3& rhs)
			{
				x *= rhs.x, y *= rhs.y;
				z *= rhs.z;
			}
			void operator/=(const Vector3& rhs)
			{
				x /= rhs.x, y /= rhs.y;
				z /= rhs.z;
			}

			Vector3 operator+(const T& rhs) const
			{
				return Vector3(x + rhs, y + rhs, z + rhs);
			}
			Vector3 operator-(const T& rhs) const
			{
				return Vector3(x - rhs, y - rhs, z - rhs);
			}
			Vector3 operator*(const T& rhs) const
			{
				return Vector3(x * rhs, y * rhs, z * rhs);
			}
			Vector3 operator/(const T& rhs) const
			{
				return Vector3(x / rhs, y / rhs, z / rhs);
			}
			void operator+=(const T& rhs)
			{
				x += rhs, y += rhs;
				z += rhs;
			}
			void operator-=(const T& rhs)
			{
				x -= rhs, y -= rhs;
				z -= rhs;
			}
			void operator*=(const T& rhs)
			{
				x *= rhs, y *= rhs;
				z *= rhs;
			}
			void operator/=(const T& rhs)
			{
				x /= rhs, y /= rhs;
				z /= rhs;
			}

			Vector3& operator++()
			{
				++x;
				++y;
				++z;
				return *this;
			}
			Vector3 operator++(int)
			{
				Vector3 result(*this);
				++(*this);
				return result;
			}

			Vector3& operator--()
			{
				--x;
				--y;
				--z;
				return *this;
			}
			Vector3 operator--(int)
			{
				Vector3 result(*this);
				--(*this);
				return result;
			}

			friend std::ostream& operator<<(std::ostream&  os,
			                                const Vector3& vec)
			{
				os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
				return os;
			}
		};
	} // namespace detail

	// Hasher to use when using Vector3 inside a hash map.
	struct Vector3Hasher
	{
		template <typename T>
		size_t operator()(const detail::Vector3<T>& k) const
		{
			return std::hash<T>()(k.x) ^ std::hash<T>()(k.y) ^
			       std::hash<T>()(k.z);
		}
	};

	// 'KeyEqual' parameter to use when using Vector3 inside a hash map.
	struct Vector3KeyComparator
	{
		template <typename T>
		bool operator()(const detail::Vector3<T>& a,
		                const detail::Vector3<T>& b) const
		{
			return a.x == b.x && a.y == b.y && a.z == b.z;
		}
	};

	// 'Key' parameter to use when using Vector3 inside a map.
	struct Vector3Key
	{
		template <typename T>
		bool operator()(const detail::Vector3<T>& a,
		                const detail::Vector3<T>& b) const
		{
			if (a.x != b.x)
			{
				return a.x < b.x;
			}
			if (a.y != b.y)
			{
				return a.y < b.y;
			}
			if (a.z != b.z)
			{
				return a.z < b.z;
			}
			return false;
		}
	};
} // namespace phx::math

template <typename T>
phx::math::detail::Vector3<T> operator-(
    const phx::math::detail::Vector3<T>& left,
    const phx::math::detail::Vector3<T>& right)
{
	return {left.x - right.x, left.y - right.y, left.z - right.z};
}

template <typename T>
phx::math::detail::Vector3<T> operator+(
    const phx::math::detail::Vector3<T>& left,
    const phx::math::detail::Vector3<T>& right)
{
	return {left.x + right.x, left.y + right.y, left.z + right.z};
}

template <typename T>
phx::math::detail::Vector3<T> operator*(
    const phx::math::detail::Vector3<T>& left,
    const phx::math::detail::Vector3<T>& right)
{
	return {left.x * right.x, left.y * right.y, left.z * right.z};
}

template <typename T>
phx::math::detail::Vector3<T> operator/(
    const phx::math::detail::Vector3<T>& left,
    const phx::math::detail::Vector3<T>& right)
{
	return {left.x / right.x, left.y / right.y, left.z / right.z};
}
