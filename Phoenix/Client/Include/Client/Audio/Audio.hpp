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

#include <Client/Audio/Source.hpp>
#include <Client/Audio/Listener.hpp>

#include <AL/alc.h>
#include <minimp3/minimp3.h>

#include <unordered_map>

namespace phx::audio
{
	struct AudioData
	{
		unsigned int buffer;
		Duration     duration;
	};
	
	class Audio
	{
	public:
		Audio() = default;
		~Audio();
		
		static bool initialize();
		static void teardown();

		void loadMP3(const std::string& uniqueName, const std::string& filePath);
		AudioData getAudioData(const std::string& uniqueName) const;

		Listener* getListener() { return &m_listener; }

	private:
		static ALCcontext* m_context;
		static ALCdevice*  m_device;
		static mp3dec_t    m_mp3;

		Listener m_listener;

		// maps uniqueName from loadMP3 to a buffer id.
		std::unordered_map<std::string, AudioData> m_buffers;
	};
} // namespace phx::audio
