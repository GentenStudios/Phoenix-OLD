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

#include <Common/PlayerView.hpp>

using namespace phx;

std::vector<voxels::Chunk> PlayerView::update(entt::registry* registry,
                                              entt::entity    entity,
                                              voxels::Map*    map)
{
	std::vector<voxels::Chunk> newChunks;

	if (!registry->has<PlayerView>(entity))
	{
		registry->emplace<PlayerView>(entity);
	}

	PlayerView& view = registry->get<PlayerView>(entity);

	// this gets the raw player position in voxel-world coordinates.
	math::vec3 playerPos =
	    (registry->get<Position>(entity).position / 2.f) + 0.5f;
	const int posX = static_cast<int>(playerPos.x) / voxels::Chunk::CHUNK_WIDTH;
	const int posY =
	    static_cast<int>(playerPos.y) / voxels::Chunk::CHUNK_HEIGHT;
	const int posZ = static_cast<int>(playerPos.z) / voxels::Chunk::CHUNK_DEPTH;

	// TODO move this to a config or add as a parameter
	const int viewDistance = 3;

	// TODO remove chunks that are out of view

	for (int x = -viewDistance; x <= viewDistance; x++)
	{
		for (int y = -viewDistance; y <= viewDistance; y++)
		{
			for (int z = -viewDistance; z <= viewDistance; z++)
			{
				math::vec3 chunkToCheck = {static_cast<float>(x + posX),
				                           static_cast<float>(y + posY),
				                           static_cast<float>(z + posZ)};
				bool       hasChunk     = false;
				for (const auto chunk : view.chunks)
				{
					if (chunk == chunkToCheck)
					{
						hasChunk = true;
						break;
					}
				}
				if (!hasChunk)
				{
					view.chunks.emplace_back(chunkToCheck);
					newChunks.emplace_back(map->getChunk(chunkToCheck));
				}
			}
		}
	}
	return newChunks;
};
