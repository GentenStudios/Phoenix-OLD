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

#include <Common/Voxels/Block.hpp>
#include <Common/Registry.hpp>

#include <string>

namespace phx::voxels
{
	struct BlockReferrer
	{
		BlockReferrer()
		{
			// this will be 0, which is equivalent to BlockType::UNKNOWN_BLOCK
			auto              uid = referrer.size();
			voxels::BlockType unknown;
			unknown.displayName      = "Unknown Block";
			unknown.id               = "core:unknown";
			unknown.category         = voxels::BlockCategory::SOLID;
			unknown.uniqueIdentifier = uid;
			referrer.add(unknown.id, uid);
			blocks.add(uid, unknown);

			// this will be 1, which is equivalent to
			// BlockType::OUT_OF_BOUNDS_BLOCK.
			uid = referrer.size();
			voxels::BlockType outOfBounds;
			outOfBounds.displayName      = "Out of Bounds";
			outOfBounds.id               = "core:out_ouf_bounds";
			outOfBounds.category         = voxels::BlockCategory::AIR;
			outOfBounds.uniqueIdentifier = uid;
			referrer.add(outOfBounds.id, uid);
			blocks.add(uid, outOfBounds);

			// this will be 2, which is equivalent to BlockType::AIR_BLOCK.
			uid = referrer.size();
			voxels::BlockType air;
			air.displayName      = "Air";
			air.id               = "core:air";
			air.category         = voxels::BlockCategory::AIR;
			air.uniqueIdentifier = uid;
			referrer.add(air.id, uid);
			blocks.add(uid, air);

			// by default will return unknown blocks if they don't exist.
			// a tiny bit hacky but solves a lot of problems without writing a
			// bunch of unnecessary code.
			referrer.setUnknownReturnVal(referrer.get("core:unknown"));
			blocks.setUnknownReturnVal(
			    blocks.get(voxels::BlockType::UNKNOWN_BLOCK));
		}

		// referrer refers string to int, which in turn is used to get the
		// blocktype.
		Registry<std::string, std::size_t> referrer;
		Registry<std::size_t, BlockType>   blocks;
	};
} // namespace phx::voxels
