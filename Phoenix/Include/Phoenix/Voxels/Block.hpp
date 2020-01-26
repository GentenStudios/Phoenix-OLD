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

#include <Phoenix/Math/Math.hpp>

#include <functional>
#include <array>
#include <string>

namespace phx
{
	namespace voxels
	{
		enum class BlockCategory : unsigned int
		{
			AIR,
			SOLID,
			LIQUID
		};

		using BlockCallback = std::function<void(math::vec3 pos)>;

		class BlockType
		{
		public:
			BlockType()  = default;
			~BlockType() = default;

			std::string displayName;
			std::string id;

			BlockCategory category = BlockCategory::AIR;

			BlockCallback onPlace;
			BlockCallback onBreak;
			BlockCallback onInteract;

			// front, left, back, right, top, bottom
			std::array<std::string, 6> textures;

			void setAllTextures(const std::string& tex) { textures.fill(tex); }

			std::size_t getRegistryID() const { return m_registryID; }

			bool operator==(const BlockType& rhs) const
			{
				return (id == rhs.id);
			}

		private:
			std::size_t m_registryID = -1;
			friend class BlockRegistry;
		};

		struct BlockMetadata
		{
			math::vec3  blockPos;
			std::string data; // serialised data, we can decide how we wanna do
			                  // this some other time (just an implementation
			                  // attempt to stop pains later in life.)
		};
	} // namespace voxels
} // namespace q2
