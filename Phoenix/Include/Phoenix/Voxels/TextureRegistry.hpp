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

#include <string>
#include <unordered_set>
#include <vector>

namespace phx
{
	namespace voxels
	{
		class TextureRegistry
		{
		public:
			void                     addTexture(const std::string& texture);
			std::vector<std::string> getTextures();

		private:
			// NOTE: We could have used an std::vector in this case (as done in
			// the BlockRegistry)
			//
			// if (std::find(m_textures.begin(), m_textures.end(), texture) ==
			// m_textures.end())
			// {
			//   m_textures.push_back(texture);
			// }
			//
			// Here we are performing a search every time we insert an element,
			// so each one by itself is upto O(N) complexity, resulting in an
			// O(N*N) complexity overall. However, std::unordered_set has a
			// constant time look-up, giving the overall ~O(N) complexity.
			// - @beeperdeeper089
			std::unordered_set<std::string> m_textures;
		};
	} // namespace voxels
} // namespace q2
