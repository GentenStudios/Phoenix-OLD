//
// Created by Nicolas Ricard on 23/03/2020.
//

#ifndef TEMPLATE_PROJECT_NAME_ENDIAN_HPP
#define TEMPLATE_PROJECT_NAME_ENDIAN_HPP

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

#ifndef __clang__
#	ifdef _MSC_VER
#		include <Windows.h>
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
	enum class endian
	{
		little  = __LITTLE_ENDIAN__,
		big     = __BIG_ENDIAN__,
		network = __BIG_ENDIAN__,
		pdp     = __PDP_ENDIAN__,
		native  = __BYTE_ORDER__
	};

	template <typename T, phx::endian from = phx::endian::native>
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
			if constexpr (from != phx::endian::big)
			{
				swap_endian();
			}
			return value.numeric;
		}
		inline T to_little()
		{
			if constexpr (from != phx::endian::little)
			{
				swap_endian();
			}
			return value.numeric;
		}
		inline T from_network()
		{
			if constexpr (from != phx::endian::network)
			{
				swap_endian();
			}
			return value.numeric;
		}
		inline T to_network()
		{
			if constexpr (from != phx::endian::network)
			{
				swap_endian();
			}
			return value.numeric;
		}
		inline T to_native()
		{
			if (from != phx::endian::native)
			{
				swap_endian();
			}
			return value.numeric;
		}

		void swap_endian()
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
	template <typename T, phx::endian from>
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

		inline T to_big()
		{
			if constexpr (from != phx::endian::big)
			{
				swap_endian();
			}
			return value.numeric[0];
		}
		inline T to_little()
		{
			if constexpr (from != phx::endian::little)
			{
				swap_endian();
			}
			return value.numeric[0];
		}
		inline T from_network()
		{
			if constexpr (from != phx::endian::network)
			{
				swap_endian();
			}
			return value.numeric[0];
		}
		inline T to_network()
		{
			if constexpr (from != phx::endian::network)
			{
				swap_endian();
			}
			return value.numeric[0];
		}
		inline T to_native()
		{
			if (from != phx::endian::native)
			{
				swap_endian();
			}
			return value.numeric[0];
		}

		void swap_endian()
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
T big_endian(T value_)
{
	phx::word<T, phx::endian::native> value(value_);
	return value.to_big();
}

template <typename T>
T little_endian(T value_)
{
	phx::word<T, phx::endian::native> value(value_);
	return value.to_little();
}

template <typename T>
T from_network(T value_)
{
	phx::word<T, phx::endian::big> value(value_);
	return value.from_network();
}

template <typename T>
T to_network(T value_)
{
	phx::word<T, phx::endian::native> value(value_);
	return value.to_network();
}

#endif // TEMPLATE_PROJECT_NAME_ENDIAN_HPP
