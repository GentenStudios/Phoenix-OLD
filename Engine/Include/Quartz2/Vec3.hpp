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
#include <ostream>

namespace q2
{
	template <typename T>
	class TVec3
	{
	public:
		using ValueType           = T;
		using ValueReference      = T &;
		using ConstValueReference = const T &;

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

		constexpr TVec3() : x(), y(), z() {}

		template <typename X>
		explicit constexpr TVec3(X val)
			: x(static_cast<T>(val)), y(static_cast<T>(val)),
			z(static_cast<T>(val))
		{
		}

		template <typename X, typename Y, typename Z>
		constexpr TVec3(X x, Y y, Z z)
			: x(static_cast<T>(x)), y(static_cast<T>(y)),
			z(static_cast<T>(z))
		{
		}

		template <typename X>
		TVec3(const TVec3<X>& other)
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			z = static_cast<T>(other.z);
		}

		template <typename X>
		TVec3& operator=(const TVec3<X>& other)
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			z = static_cast<T>(other.z);

			return *this;
		}

		template <typename NewType>
		explicit constexpr operator TVec3<NewType>() const
		{
			TVec3<NewType> vec;

			vec.x = static_cast<NewType>(x);
			vec.y = static_cast<NewType>(y);
			vec.z = static_cast<NewType>(z);

			return vec;
		}

		void floor()
		{
			std::floor(x);
			std::floor(y);
			std::floor(z);
		}
		void ceil()
		{
			std::ceil(x);
			std::ceil(y);
			std::ceil(z);
		}

		void normalize()
		{
			const float magnitude = std::sqrt(dotProduct(*this, *this));

			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
		}

		static TVec3 normalize(const TVec3& vec)
		{
			const float magnitude = std::sqrt(dotProduct(vec, vec));

			return TVec3(vec.x / magnitude, vec.y / magnitude,
				vec.z / magnitude);
		}

		static TVec3 cross(const TVec3& vec1, const TVec3& vec2)
		{
			return TVec3(vec1.y * vec2.z - vec1.z * vec2.y,
				vec1.z * vec2.x - vec1.x * vec2.z,
				vec1.x * vec2.y - vec1.y * vec2.x);
		}

		static float dotProduct(const TVec3& vec1,
			const TVec3& vec2)
		{
			return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
		}

		bool operator==(const TVec3& rhs) const
		{
			return (x == rhs.x && y == rhs.y && z == rhs.z);
		}
		bool operator!=(const TVec3& rhs) const
		{
			return (x != rhs.x && y != rhs.y && z != rhs.z);
		}
		bool operator>(const TVec3& rhs) const
		{
			return (x > rhs.x && y > rhs.y && z > rhs.z);
		}
		bool operator<(const TVec3& rhs) const
		{
			return (x < rhs.x && y < rhs.y && z < rhs.z);
		}
		bool operator>=(const TVec3& rhs) const
		{
			return (x >= rhs.x && y >= rhs.y && z >= rhs.z);
		}
		bool operator<=(const TVec3& rhs) const
		{
			return (x <= rhs.x && y <= rhs.y && z <= rhs.z);
		}
		
		TVec3 operator+(const TVec3& rhs) const
		{
			return TVec3(x + rhs.x, y + rhs.y, z + rhs.z);
		}
		TVec3 operator-(const TVec3& rhs) const
		{
			return TVec3(x - rhs.x, y - rhs.y, z - rhs.z);
		}
		TVec3 operator*(const TVec3& rhs) const
		{
			return TVec3(x * rhs.x, y * rhs.y, z * rhs.z);
		}
		TVec3 operator/(const TVec3& rhs) const
		{
			return TVec3(x / rhs.x, y / rhs.y, z / rhs.z);
		}
		void operator+=(const TVec3& rhs)
		{
			x += rhs.x, y += rhs.y;
			z += rhs.z;
		}
		void operator-=(const TVec3& rhs)
		{
			x -= rhs.x, y -= rhs.y;
			z -= rhs.z;
		}
		void operator*=(const TVec3& rhs)
		{
			x *= rhs.x, y *= rhs.y;
			z *= rhs.z;
		}
		void operator/=(const TVec3& rhs)
		{
			x /= rhs.x, y /= rhs.y;
			z /= rhs.z;
		}

		TVec3 operator+(const T& rhs) const
		{
			return TVec3(x + rhs, y + rhs, z + rhs);
		}
		TVec3 operator-(const T& rhs) const
		{
			return TVec3(x - rhs, y - rhs, z - rhs);
		}
		TVec3 operator*(const T& rhs) const
		{
			return TVec3(x * rhs, y * rhs, z * rhs);
		}
		TVec3 operator/(const T& rhs) const
		{
			return TVec3(x / rhs, y / rhs, z / rhs);
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

		TVec3& operator++()
		{
			++x;
			++y;
			++z;
			return *this;
		}
		TVec3 operator++(int)
		{
			TVec3 result(*this);
			++(*this);
			return result;
		}

		TVec3& operator--()
		{
			--x;
			--y;
			--z;
			return *this;
		}
		TVec3 operator--(int)
		{
			TVec3 result(*this);
			--(*this);
			return result;
		}

		friend std::ostream& operator<<(std::ostream& os,
			const TVec3& vec)
		{
			os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
			return os;
		}
	};

	using Vec3  = TVec3<float>;
	using Vec3f = TVec3<float>;
	using Vec3d = TVec3<double>;
	using Vec3i = TVec3<int>;
	using Vec3u = TVec3<unsigned int>;
} // namespace q2

template <typename T>
q2::TVec3<T> operator-(
	const q2::TVec3<T>& left,
	const q2::TVec3<T>& right)
{
	return { left.x - right.x, left.y - right.y, left.z - right.z };
}

template <typename T>
q2::TVec3<T> operator+(
	const q2::TVec3<T>& left,
	const q2::TVec3<T>& right)
{
	return { left.x + right.x, left.y + right.y, left.z + right.z };
}

template <typename T>
q2::TVec3<T> operator*(
	const q2::TVec3<T>& left,
	const q2::TVec3<T>& right)
{
	return { left.x * right.x, left.y * right.y, left.z * right.z };
}

template <typename T>
q2::TVec3<T> operator/(
	const q2::TVec3<T>& left,
	const q2::TVec3<T>& right)
{
	return { left.x / right.x, left.y / right.y, left.z / right.z };
}

