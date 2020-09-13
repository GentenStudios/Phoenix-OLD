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

namespace phx::gfx
{
	/**
	 * @brief Each face of one of the default block models.
	 *
	 * This is the order in which textures in Lua MUST be registered. The
	 * system only understands this format, or textures will be the wrong
	 * way round and you won't know why.
	 */
	enum class BlockFace : unsigned int
	{
		NORTH = 0,
		EAST,
		SOUTH,
		WEST,
		TOP,
		BOTTOM
	};
	
	// supports some default block models, and customs too.
	enum class BlockModel
	{
		BLOCK,
		SLAB,
		SLOPE,
		STAIR,
		X_PANEL,
		X_PANEL_CUBE,
	};

	inline const char* blockModelToString(BlockModel model)
	{
		switch (model)
		{
		case BlockModel::BLOCK:
			return "Block";
		case BlockModel::SLAB:
			return "Slab";
		case BlockModel::SLOPE:
			return "Slope";
		case BlockModel::STAIR:
			return "Stair";
		case BlockModel::X_PANEL:
			return "XPanel";
		case BlockModel::X_PANEL_CUBE:
			return "XPanelCube";
		}

		// don't need this here but clang-tidy keeps complaining and we don't
		// lose anything.
		return "";
	}
} // namespace phx::gfx
