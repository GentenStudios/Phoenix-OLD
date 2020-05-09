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
// Copyright 2019 ENGINEnten Studios
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
// CONSEQUENTIAL DAMAENGINES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIENGINENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAENGINE.

#pragma once

#if defined(_DEBUG)
#	define ENGINE_DEBUG
#endif

// Determine the compiler and set the corresponding defines for it.
#ifdef _MSC_VER
#	define ENGINE_MSVC
#elif defined(__clang__)
#	define ENGINE_CLANG
#elif defined(__GNUC__)
#	define ENGINE_GNUC
#endif

// Determine the OS and set the corresponding defines for it.
#if defined(_WIN32) || defined(_WIN64)
#	define ENGINE_PLATFORM_WINDOWS
#elif defined(__linux__)
#	define ENGINE_PLATFORM_LINUX
#elif defined(__APPLE__)
#	define ENGINE_PLATFORM_APPLE
#endif

#if defined(ENGINE_PLATFORM_WINDOWS)
#	define IF_WINDOWS(x) x
#else
#	define IF_WINDOWS(x)
#endif

#if defined(ENGINE_PLATFORM_LINUX)
#	define IF_LINUX(x) x
#else
#	define IF_LINUX(x)
#endif

#if defined(ENGINE_PLATFORM_APPLE)
#	define IF_APPLE(x) x
#else
#	define IF_APPLE(x)
#endif

#if defined(ENGINE_MSVC)
#	define ENGINE_FORCE_INLINE __forceinline
#	define ENGINE_NORETURN __declspec(noreturn)
#elif defined(ENGINE_CLANG) || defined(ENGINE_GNUC)
#	define ENGINE_FORCE_INLINE __attribute__((always_inline))
#	define ENGINE_NORETURN __attribute__((noreturn))
#endif

#if defined(ENGINE_DEBUG)
#	if defined(ENGINE_MSVC)
#		define BREAKPOINT() __debugbreak()
#	elif defined(ENGINE_CLANG) || defined(PC_GNUC)
#		define BREAKPOINT() __builtin_trap()
#	endif
#else
#	define BREAKPOINT()
#endif

// TODO: (beeperdeeper089) Log the assertion fail & allow all the extra arguments to go
#if defined(ENGINE_DEBUG)
#	define ENGINE_ASSERT(condition, ...) \
		{                             \
			assert(condition);        \
		}
#else
#	define ENGINE_ASSERT(condition, ...)
#	define NDEBUG // DISABLE ASSERTIONS.
#endif
