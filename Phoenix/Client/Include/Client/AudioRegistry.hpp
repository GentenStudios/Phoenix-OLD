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

#include <Common/CMS/ModManager.hpp>
#include <Common/Registry.hpp>

#include <string>

#include <soloud.h>
#include <soloud_wav.h>

namespace phx::client
{
	using SourceGroup = std::vector<SoLoud::Wav*>;

	class AudioRegistry
	{
	public:
		void registerAPI(cms::ModManager* manager, SoLoud::Soloud* soloud);

		std::size_t add(const std::string& path, const std::string& id);

		/**
		 * @brief Gets an Audio Source from the registry by its string ID.
		 * @param id The string ID of the Audio Source.
		 * @return Pointer to the Audio Source matching the string ID.
		 *
		 * @note The stringID only exists to persist data between runtimes, for
		 * operations that exist within the scope of runtime, the numerical ID
		 * should be used instead to enhance performance.
		 */
		SoLoud::Wav* getByID(const std::string& id)
		{
			return sources.get(*referrer.get(id));
		};

		/**
		 * @brief Gets an Audio Source from the registry by its numerical ID.
		 * @param id The numerical ID of the Audio Source.
		 * @return Pointer to the Audio Source matching the numerical ID.
		 *
		 * @note The numerical ID is assigned on registration and does not
		 * persist beyond runtime. This value should be used during runtime for
		 * performance but never saved.
		 */
		SoLoud::Wav* get(std::size_t id) { return sources.get(id); };

	private:
		// referrer refers a string to int, which in turn is used to get the
		// source.
		Registry<std::string, std::size_t> referrer;
		Registry<std::size_t, SoLoud::Wav> sources;
	};
} // namespace phx::client