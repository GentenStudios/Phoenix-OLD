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

#include <Client/Audio/Listener.hpp>

#include <AL/al.h>

using namespace phx::audio;

void Listener::setGain(float gain) { alListenerf(AL_GAIN, gain); }

void Listener::setPosition(phx::math::vec3 position)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);

	float data[3];
	alGetListener3f(AL_POSITION, &data[0], &data[1], &data[2]);
}

void Listener::setVelocity(phx::math::vec3 velocity)
{
	alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void Listener::setOrientation(phx::math::vec3 direction, phx::math::vec3 up)
{
	float orientation[6] = {direction.x, direction.y, direction.z,
	                        -up.x,       -up.y,       -up.z};
	alListenerfv(AL_ORIENTATION, orientation);
}
