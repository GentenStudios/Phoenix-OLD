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

#ifndef TEMPLATE_PROJECT_NAME_ENDIAN_HPP
#define TEMPLATE_PROJECT_NAME_ENDIAN_HPP

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#ifndef __clang__
#	ifdef _MSC_VER
#		define __LITTLE_ENDIAN__ 1
#		define __BIG_ENDIAN__ 2
#		define __PDP_ENDIAN__ 3
#		if REG_DWORD == REG_DWORD_LITTLE_ENDIAN
#			define __BYTE_ORDER__ __LITTLE_ENDIAN__
#		else
#			define __BYTE_ORDER__ __BIG_ENDIAN__
#		endif
#	endif
#endif

#ifndef __LITTLE_ENDIAN__
#	if defined(__ORDER_LITTLE_ENDIAN__)
#		define __LITTLE_ENDIAN__ __ORDER_LITTLE_ENDIAN__
#	elif defined(__LITTLE_ENDIAN)
#		define __LITTLE_ENDIAN__ __LITTLE_ENDIAN
#	elif defined(_LITTLE_ENDIAN)
#		define __LITTLE_ENDIAN__ _LITTLE_ENDIAN
#	elif defined(G_LITTLE_ENDIAN)
#		define __LITTLE_ENDIAN__ G_LITTLE_ENDIAN
#	endif
#endif

#ifndef __BIG_ENDIAN__
#	if defined(__ORDER_BIG_ENDIAN__)
#		define __BIG_ENDIAN__ __ORDER_BIG_ENDIAN__
#	elif defined(__BIG_ENDIAN)
#		define __BIG_ENDIAN__ __BIG_ENDIAN
#	elif defined(_BIG_ENDIAN)
#		define __BIG_ENDIAN__ _BIG_ENDIAN
#	elif defined(G_BIG_ENDIAN)
#		define __BIG_ENDIAN__ G_BIG_ENDIAN
#	endif
#endif

#ifndef __PDP_ENDIAN__
#	if defined(__ORDER_PDP_ENDIAN__)
#		define __PDP_ENDIAN__ __ORDER_PDP_ENDIAN__
#	elif defined(__PDP_ENDIAN)
#		define __PDP_ENDIAN__ __PDP_ENDIAN
#	elif defined(_PDP_ENDIAN)
#		define __PDP_ENDIAN__ _PDP_ENDIAN
#	elif defined(G_PDP_ENDIAN)
#		define __PDP_ENDIAN__ G_PDP_ENDIAN
#	endif
#endif

#ifndef __BYTE_ORDER__
#	if defined(__ORDER_ENDIAN__)
#		define __BYTE_ORDER__ __ORDER_ENDIAN__
#	elif defined(__BYTE_ORDER)
#		define __BYTE_ORDER__ __BYTE_ORDER
#	elif defined(_BYTE_ORDER)
#		define __BYTE_ORDER__ _BYTE_ORDER
#	elif defined(G_BYTE_ORDER)
#		define __BYTE_ORDER__ G_BYTE_ORDER
#	endif
#endif

namespace phx
{
	enum class Endian
	{
		LITTLE  = __LITTLE_ENDIAN__,
		BIG     = __BIG_ENDIAN__,
		NETWORK = __BIG_ENDIAN__,
		PDP     = __PDP_ENDIAN__,
		NATIVE  = __BYTE_ORDER__
	};

	template <typename T, phx::Endian from = phx::Endian::NATIVE>
	struct word
	{
		static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");
		std::size_t size;
		union value_type
		{
			std::byte bytes[sizeof(T)];
			T         data[1];
			T         numeric;
		} value;

		explicit word(T const& value_)
		{
			value.numeric = value_;
			size          = sizeof(T);
		}

		explicit word(std::byte const* bytes_, const std::size_t size_)
		{
			size = size_ * sizeof(T);
			std::memcpy(value.bytes, bytes_, size_ * sizeof(T));
		}

		inline T to_big()
		{
			if constexpr (from != phx::Endian::big)
			{
				swapEndian();
			}
			return value.numeric;
		}
		inline T toLittle()
		{
			if constexpr (from != phx::Endian::little)
			{
				swapEndian();
			}
			return value.numeric;
		}
		inline T from_network()
		{
			if constexpr (from != phx::Endian::NETWORK)
			{
				swapEndian();
			}
			return value.numeric;
		}
		inline T to_network()
		{
			if constexpr (from != phx::Endian::NETWORK)
			{
				swapEndian();
			}
			return value.numeric;
		}
		inline T to_native()
		{
			if (from != phx::Endian::NATIVE)
			{
				swapEndian();
			}
			return value.numeric;
		}

		void swapEndian()
		{
			if constexpr (sizeof(T) > 1)
			{
				for (size_t init = 0; init < size / sizeof(T); ++init)
				{
					for (size_t k = 0; k < sizeof(T) / 2; ++k)
					{
						std::swap(
						    value.bytes[k],
						    value.bytes[init * sizeof(T) + sizeof(T) - k - 1]);
					}
				}
			}
		}
	};
	template <typename T, phx::Endian from>
	struct word<T*, from>
	{
		static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");
		std::size_t size;
		union value_type
		{
			std::byte* bytes;
			T*         data;
			T*         numeric;
		} value;

		explicit word(std::byte* bytes_, const std::size_t size_)
		{
			size        = size_ * sizeof(T);
			value.bytes = bytes_;
		}

		inline T toBig()
		{
			if constexpr (from != phx::Endian::BIG)
			{
				swapEndian();
			}
			return value.numeric[0];
		}
		inline T toLittle()
		{
			if constexpr (from != phx::Endian::LITTLE)
			{
				swapEndian();
			}
			return value.numeric[0];
		}
		inline T from_network()
		{
			if constexpr (from != phx::Endian::NETWORK)
			{
				swapEndian();
			}
			return value.numeric[0];
		}
		inline T to_network()
		{
			if constexpr (from != phx::Endian::NETWORK)
			{
				swapEndian();
			}
			return value.numeric[0];
		}
		inline T to_native()
		{
			if (from != phx::Endian::NATIVE)
			{
				swapEndian();
			}
			return value.numeric[0];
		}

		void swapEndian()
		{
			if constexpr (sizeof(T) > 1)
			{
				for (size_t init = 0; init < size / sizeof(T); ++init)
				{
					for (size_t k = 0; k < sizeof(T) / 2; ++k)
					{
						std::swap(
						    value.bytes[k],
						    value.bytes[init * sizeof(T) + sizeof(T) - k - 1]);
					}
				}
			}
		}
	};
} // namespace phx

template <typename T>
T bigEndian(T value_)
{
	phx::word<T, phx::Endian::NATIVE> value(value_);
	return value.toBig();
}

template <typename T>
T littleEndian(T value_)
{
	phx::word<T, phx::Endian::NATIVE> value(value_);
	return value.toLittle();
}

template <typename T>
T fromNetwork(T value_)
{
	phx::word<T, phx::Endian::BIG> value(value_);
	return value.from_network();
}

template <typename T>
T toNetwork(T value_)
{
	phx::word<T, phx::Endian::NATIVE> value(value_);
	return value.to_network();
}

#endif // TEMPLATE_PROJECT_NAME_ENDIAN_HPP
