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

#include <Common/CoreIntrinsics.hpp>
#include <Common/Serialization/SharedTypes.hpp>

#include <cstddef>
#include <cstdint>

// for potential GCC defines.
#include <limits.h>

// by the end of this preprocessor cluster fuck, there should be an
// ENGINE_NATIVE_ENDIAN that is defined. The static assert in the namespace will
// make sure it's not ENGINE_UNKNOWN_ENDIAN, since if we cannot determine the
// platform's endianness we do be in a bit of a pickle.

#define ENGINE_UNKNOWN_ENDIAN -1
#define ENGINE_LITTLE_ENDIAN 0
#define ENGINE_BIG_ENDIAN 1
#define ENGINE_NET_ENDIAN ENGINE_BIG_ENDIAN

// c++17 feature, C++11 extension in CLang, GCC 5+, VS2015+
// you should be fine even without it.
#ifdef __has_include
#	if __has_include(<endian.h>)
#		include <endian.h> // gnu libc normally provides, linux
#	elif __has_include(<machine/endian.h>)
#		include <machine/endian.h> //open bsd, macos
#	elif __has_include(<sys/param.h>)
#		include <sys/param.h> // mingw, some bsd (not open/macos)
#	elif __has_include(<sys/isadefs.h>)
#		include <sys/isadefs.h> // solaris
#	endif
#endif
#if defined(__BIG_ENDIAN__)
#	define ENGINE_NATIVE_ENDIAN ENGINE_BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN__)
#	define ENGINE_NATIVE_ENDIAN ENGINE_LITTLE_ENDIAN
#endif

// this should exhaust literally every possibility.
#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#	if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || \
	    (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) ||             \
	    (defined(_BYTE_ORDER) && _BYTE_ORDER == _BIG_ENDIAN) ||                \
	    (defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN) ||                   \
	    (defined(__sun) && defined(__SVR4) && defined(_BIG_ENDIAN)) ||         \
	    defined(__ARMEB__) || defined(__THUMBEB__) ||                          \
	    defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB) ||     \
	    defined(__MIBSEB__) || defined(_M_PPC)
#		define ENGINE_NATIVE_ENDIAN ENGINE_BIG_ENDIAN
#	elif (defined(__BYTE_ORDER__) &&                                          \
	       __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || /* gcc */             \
	    (defined(__BYTE_ORDER) &&                                              \
	     __BYTE_ORDER == __LITTLE_ENDIAN) /* linux header */                   \
	    || (defined(_BYTE_ORDER) && _BYTE_ORDER == _LITTLE_ENDIAN) ||          \
	    (defined(BYTE_ORDER) &&                                                \
	     BYTE_ORDER == LITTLE_ENDIAN) /* mingw header */                       \
	    || (defined(__sun) && defined(__SVR4) &&                               \
	        defined(_LITTLE_ENDIAN)) || /* solaris */                          \
	    defined(__ARMEL__) ||                                                  \
	    defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) ||  \
	    defined(__MIPSEL) || defined(__MIPSEL__) || defined(_M_IX86) ||        \
	    defined(_M_X64) || defined(_M_IA64) || /* msvc for intel processors */ \
	    defined(_M_ARM) /* msvc code on arm executes in little endian mode */
#		define ENGINE_NATIVE_ENDIAN ENGINE_LITTLE_ENDIAN
#	endif
#endif

// something is wrong, it hasn't been defined yet.
#ifndef ENGINE_NATIVE_ENDIAN
#	define ENGINE_NATIVE_ENDIAN ENGINE_UNKNOWN_ENDIAN
#	warning \
	    "Unknown platform endianness. Network/Host byte swaps will not occur - bugs may be present."
#endif

// define builtin byte swapping methods, much faster than custom methods.
// will work for signed as well.
// clang-format off
#if defined(ENGINE_MSVC)
#	define byteswap16(x) _byteswap_ushort((x))
#	define byteswap32(x) _byteswap_ulong((x))
#	define byteswap64(x) _byteswap_uint64((x))
#elif defined(ENGINE_GNUC) && \
    ((__GNUC__ >= 5) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
#	define byteswap16(x) __builtin_bswap16((x))
#	define byteswap32(x) __builtin_bswap32((x))
#	define byteswap64(x) __builtin_bswap64((x))
#elif defined(__has_builtin) && __has_builtin(__builtin_bswap64)
#	define byteswap16(x) __builtin_bswap16((x))
#	define byteswap32(x) __builtin_bswap32((x))
#	define byteswap64(x) __builtin_bswap64((x))
#else
	// compilers will most likely optimise this out - even if the builtin's don't
	// exist.
	static inline uint16_t byteswap16(uint16_t x)
	{
		return (((x >> 8) & 0xffu) | ((x & 0xffu) << 8));
	}
	static inline uint32_t byteswap32(uint32_t x)
	{
		return (((x & 0xff000000u) >> 24) | ((x & 0x00ff0000u) >> 8) |
		        ((x & 0x0000ff00u) << 8) | ((x & 0x000000ffu) << 24));
	}
	static inline uint64_t byteswap64(uint64_t x)
	{
		return (((x & 0xff00000000000000ull) >> 56) |
		        ((x & 0x00ff000000000000ull) >> 40) |
		        ((x & 0x0000ff0000000000ull) >> 24) |
		        ((x & 0x000000ff00000000ull) >> 8) |
		        ((x & 0x00000000ff000000ull) << 8) |
		        ((x & 0x0000000000ff0000ull) << 24) |
		        ((x & 0x000000000000ff00ull) << 40) |
		        ((x & 0x00000000000000ffull) << 56));
	}
#endif
// clang-format on

#include <type_traits>

namespace phx::data::endian
{
	enum class Endian
	{
		LITTLE = ENGINE_LITTLE_ENDIAN,
		BIG    = ENGINE_BIG_ENDIAN,
		NET    = ENGINE_NET_ENDIAN,
		NATIVE = ENGINE_NATIVE_ENDIAN
	};

	namespace detail
	{
		template <typename T>
		struct IsEndianChangable
		{
			static constexpr bool value = std::is_integral_v<T> || std::is_floating_point_v<T>;
		};

		template <std::size_t N>
		struct ByteSwapper
		{
		};

		// you don't swap the byte order if there's only one byte.
		template <>
		struct ByteSwapper<1>
		{
			template <typename T>
			T operator()(const T& t)
			{
				return t;
			}
		};

		template <>
		struct ByteSwapper<2>
		{
			template <typename T>
			T operator()(const T& t)
			{
				return byteswap16(t);
			}
		};

		template <>
		struct ByteSwapper<4>
		{
			template <typename T>
			T operator()(const T& t)
			{
				return byteswap32(t);
			}

			// special operator for float since it needs to be turned into an
			// integer first.
			float operator()(float f)
			{
				uint64_t t = byteswap32(*reinterpret_cast<const uint32_t*>(&f));
				return *reinterpret_cast<const float*>(&t);
			}
		};

		template <>
		struct ByteSwapper<8>
		{
			template <typename T>
			T operator()(const T& t)
			{
				return byteswap64(t);
			}

			// special operator for double since it needs to be turned into an
			// integer first.
			double operator()(double d)
			{
				uint64_t t = byteswap64(*reinterpret_cast<const uint64_t*>(&d));
				return *reinterpret_cast<const double*>(&t);
			}
		};
	} // namespace detail

	/**
	 * @brief Swaps the endianness of data for sending over a network.
	 * @tparam T The type of data being converted. (must be integral type)
	 * @tparam from The endianness that the data currently is.
	 * @param t The value to check/swap for endianness
	 * @return The endian-safe value.
	 */
	template <typename T, Endian from = Endian::NATIVE,
	          typename U =
	              std::enable_if_t<detail::IsEndianChangable<T>::value, void>>
	T swapForNetwork(const T& t)
	{
		if constexpr (from != Endian::NET)
		{
			return detail::ByteSwapper<sizeof(T)>()(t);
		}

		return t;
	}

	/**
	 * @brief Swaps the endianness of data after receiving from a network.
	 * @tparam T The type of data being converted. (must be integral type)
	 * @tparam from The endianness that the data currently is.
	 * @param t The value to check/swap for endianness
	 * @return The endian-safe value.
	 */
	template <typename T, Endian from = Endian::NET,
	          typename U =
	              std::enable_if_t<detail::IsEndianChangable<T>::value, void>>
	T swapForHost(const T& t)
	{
		if constexpr (from != Endian::NATIVE)
		{
			return detail::ByteSwapper<sizeof(T)>()(t);
		}

		return t;
	}
} // namespace phx::data::endian
