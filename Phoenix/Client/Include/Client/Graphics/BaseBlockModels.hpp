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

/**
 * @file ChunkMesher.hpp
 * @brief The mesher class for use with chunks.
 *
 * @copyright Copyright (c) 2019-20 Genten Studios
 */

#pragma once

#include <Client/Graphics/ChunkRenderer.hpp>

#include <Common/Math/Math.hpp>

namespace phx::gfx
{
	struct DefaultMeshVertex
	{
		math::vec3 pos;
		math::vec2 uv;
		math::vec3 normal;
	};
}

// clang-format off
static const phx::gfx::DefaultMeshVertex vertBlock[] = {
	// front (north)
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 0, -1)},

	// left (west)
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(-1, 0, 0)},

	// back (south)
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, 1)},

	// right (east)
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(1, 0, 0)},

	// top
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, 0)},

	// bottom
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, -1, 0)},
};

static constexpr std::size_t                        BLOCK_MAX_VERTS       = 36;
static constexpr const phx::gfx::DefaultMeshVertex* BLOCK_FRONT           = &vertBlock[0];
static constexpr const phx::gfx::DefaultMeshVertex* BLOCK_LEFT            = &vertBlock[6];
static constexpr const phx::gfx::DefaultMeshVertex* BLOCK_BACK            = &vertBlock[12];
static constexpr const phx::gfx::DefaultMeshVertex* BLOCK_RIGHT           = &vertBlock[18];
static constexpr const phx::gfx::DefaultMeshVertex* BLOCK_TOP             = &vertBlock[24];
static constexpr const phx::gfx::DefaultMeshVertex* BLOCK_BOTTOM          = &vertBlock[30];
static constexpr std::size_t                        BLOCK_FACE_VERT_COUNT = 6;

static const phx::gfx::DefaultMeshVertex vertSlab[] = {
	// front (north)
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f,  0.f, -1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f,  0.f, -1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3(-1.f,  0.f, -1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, 0, -1)},

	// left (west)
    {phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f,  0.f, -1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(-1, 0, 0)},

	// back (south)
    {phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f,  0.f,  1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 0, 1)},

	// right (east)
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  0.f, -1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  0.f,  1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  0.f,  1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(1, 0, 0)},

	// top
    {phx::math::vec3(-1.f,  0.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  0.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f,  0.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f,  0.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, 0)},

	// bottom
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, -1, 0)},
};

static constexpr std::size_t                        SLAB_MAX_VERTS       = 36;
static constexpr const phx::gfx::DefaultMeshVertex* SLAB_FRONT           = &vertSlab[0];
static constexpr const phx::gfx::DefaultMeshVertex* SLAB_LEFT            = &vertSlab[6];
static constexpr const phx::gfx::DefaultMeshVertex* SLAB_BACK            = &vertSlab[12];
static constexpr const phx::gfx::DefaultMeshVertex* SLAB_RIGHT           = &vertSlab[18];
static constexpr const phx::gfx::DefaultMeshVertex* SLAB_TOP             = &vertSlab[24];
static constexpr const phx::gfx::DefaultMeshVertex* SLAB_BOTTOM          = &vertSlab[30];
static constexpr std::size_t                        SLAB_FACE_VERT_COUNT = 6;

static const phx::gfx::DefaultMeshVertex vertStair[] = {
	// front (north)
		// lower bit
		{phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, 0, -1)},
		{phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, 0, -1)},
		{phx::math::vec3( 1.f,  0.f, -1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 0, -1)},
		{phx::math::vec3( 1.f,  0.f, -1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 0, -1)},
		{phx::math::vec3(-1.f,  0.f, -1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 0, -1)},
		{phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, 0, -1)},
		// upper bit
		{phx::math::vec3(-1.f,  0.f,  0.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 0, -1)},
		{phx::math::vec3( 1.f,  0.f,  0.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 0, -1)},
		{phx::math::vec3( 1.f,  1.f,  0.f), phx::math::vec2(0.f,  1.f),  phx::math::vec3(0, 0, -1)},
		{phx::math::vec3( 1.f,  1.f,  0.f), phx::math::vec2(0.f,  1.f),  phx::math::vec3(0, 0, -1)},
		{phx::math::vec3(-1.f,  1.f,  0.f), phx::math::vec2(1.f,  1.f),  phx::math::vec3(0, 0, -1)},
		{phx::math::vec3(-1.f,  0.f,  0.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 0, -1)},

	// left (west)
		// lower bit
		{phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f,  0.f, -1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(-1, 0, 0)},
		// upper bit
		{phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f),  phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f,  1.f,  0.f), phx::math::vec2(0.5f, 1.f),  phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f,  0.f,  0.f), phx::math::vec2(0.5f, 0.5f), phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f,  0.f,  0.f), phx::math::vec2(0.5f, 0.5f), phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f,  0.f,  1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(-1, 0, 0)},
		{phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f),  phx::math::vec3(-1, 0, 0)},

	// back (south) (only one back bit)
	{phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, 1)},
	{phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, 0, 1)},
	{phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, 0, 1)},
	{phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, 0, 1)},
	{phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 0, 1)},
	{phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, 1)},

	// right (east)
		// lower bit
		{phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f,  0.f, -1.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f,  0.f,  1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f,  0.f,  1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(1, 0, 0)},
		// upper bit
		{phx::math::vec3(-1.f,  0.f,  0.f), phx::math::vec2(0.5f, 0.5f), phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f,  1.f,  0.f), phx::math::vec2(0.5f, 1.f),  phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f),  phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f),  phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f,  0.f,  1.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(1, 0, 0)},
		{phx::math::vec3(-1.f,  0.f,  0.f), phx::math::vec2(0.5f, 0.5f), phx::math::vec3(1, 0, 0)},

	// top
		// lower bit
		{phx::math::vec3(-1.f,  0.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, 1, 0)},
		{phx::math::vec3( 1.f,  0.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, 1, 0)},
		{phx::math::vec3( 1.f,  0.f,  0.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 1, 0)},
		{phx::math::vec3( 1.f,  0.f,  0.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 1, 0)},
		{phx::math::vec3(-1.f,  0.f,  0.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 1, 0)},
		{phx::math::vec3(-1.f,  0.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, 1, 0)},
		// upper bit
		{phx::math::vec3(-1.f,  1.f,  0.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 1, 0)},
		{phx::math::vec3( 1.f,  1.f,  0.f), phx::math::vec2(0.f,  0.5f), phx::math::vec3(0, 1, 0)},
		{phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f),  phx::math::vec3(0, 1, 0)},
		{phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f),  phx::math::vec3(0, 1, 0)},
		{phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f),  phx::math::vec3(0, 1, 0)},
		{phx::math::vec3(-1.f,  1.f,  0.f), phx::math::vec2(1.f,  0.5f), phx::math::vec3(0, 1, 0)},

	// bottom (only one bottom)
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f),  phx::math::vec3(0, -1, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f),  phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f),  phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  1.f),  phx::math::vec3(0, -1, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f),  phx::math::vec3(0, -1, 0)},
};

static constexpr std::size_t                        STAIR_MAX_VERTS    = 60;
static constexpr const phx::gfx::DefaultMeshVertex* STAIR_FRONT        = &vertStair[0];
static constexpr std::size_t                        STAIR_FRONT_COUNT  = 12; // twelve vertices because there are TWO front panels.
static constexpr const phx::gfx::DefaultMeshVertex* STAIR_LEFT         = &vertStair[12];
static constexpr std::size_t                        STAIR_LEFT_COUNT   = 12; // twelve vertices because there are TWO left panels.
static constexpr const phx::gfx::DefaultMeshVertex* STAIR_BACK         = &vertStair[24];
static constexpr std::size_t                        STAIR_BACK_COUNT   = 6; // six vertices because there is only ONE back panel.
static constexpr const phx::gfx::DefaultMeshVertex* STAIR_RIGHT        = &vertStair[30];
static constexpr std::size_t                        STAIR_RIGHT_COUNT  = 12; // twelve vertices because there are TWO right panels.
static constexpr const phx::gfx::DefaultMeshVertex* STAIR_TOP          = &vertStair[42];
static constexpr std::size_t                        STAIR_TOP_COUNT    = 12; // twelve vertices because there are TWO top panels.
static constexpr const phx::gfx::DefaultMeshVertex* STAIR_BOTTOM       = &vertStair[54];
static constexpr std::size_t                        STAIR_BOTTOM_COUNT = 6; // six vertices because there is only ONE bottom panels.

static const phx::gfx::DefaultMeshVertex vertSlope[] = {
	// front (north)
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, -1)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 1, -1)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, -1)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, -1)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 1, -1)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, -1)},

	// left (west)
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(-1, 0, 0)},

	// back (south)
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, 1)},

	// right (east)
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(1, 0, 0)},

	// no top, front is the slope.

	// bottom
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, -1, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, -1, 0)},
};

static constexpr std::size_t                        SLOPE_MAX_VERTS    = 24;
static constexpr const phx::gfx::DefaultMeshVertex* SLOPE_FRONT        = &vertSlope[0];
static constexpr std::size_t                        SLOPE_FRONT_COUNT  = 6;
static constexpr const phx::gfx::DefaultMeshVertex* SLOPE_LEFT         = &vertSlope[6];
static constexpr std::size_t                        SLOPE_LEFT_COUNT   = 3;
static constexpr const phx::gfx::DefaultMeshVertex* SLOPE_BACK         = &vertSlope[9];
static constexpr std::size_t                        SLOPE_BACK_COUNT   = 6;
static constexpr const phx::gfx::DefaultMeshVertex* SLOPE_RIGHT        = &vertSlope[15];
static constexpr std::size_t                        SLOPE_RIGHT_COUNT  = 3;
static constexpr const phx::gfx::DefaultMeshVertex* SLOPE_BOTTOM       = &vertSlope[18];
static constexpr std::size_t                        SLOPE_BOTTOM_COUNT = 6;

// you will never mesh a xpanel, so no point making the helpers.
static const phx::gfx::DefaultMeshVertex vertXPan[] = {
	// east-facing front
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(-1.f, 0.f, -1.f)},

	// back of east-facing
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(1.f, 0.f, 1.f)},

	// west-facing front
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(1.f, 0.f, -1.f)},

	// back of west-facing
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(-1.f, 0.f, 1.f)},
};

static constexpr const phx::gfx::DefaultMeshVertex* XPANEL_MESH      = &vertXPan[0];
static constexpr std::size_t                        XPANEL_MAX_VERTS = 24;

// you will never mesh a xpanel (block), so no point making the helpers.
static const phx::gfx::DefaultMeshVertex vertXPanBlock[] = {
    // east-facing front
    {phx::math::vec3(-1.75f, -1.75f, -1.75f), phx::math::vec2(1.f, 0.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.75f, -1.75f,  1.75f), phx::math::vec2(0.f, 0.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.75f,  1.75f,  1.75f), phx::math::vec2(0.f, 1.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.75f,  1.75f,  1.75f), phx::math::vec2(0.f, 1.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3(-1.75f,  1.75f, -1.75f), phx::math::vec2(1.f, 1.f), phx::math::vec3(-1.f, 0.f, -1.f)},
    {phx::math::vec3(-1.75f, -1.75f, -1.75f), phx::math::vec2(1.f, 0.f), phx::math::vec3(-1.f, 0.f, -1.f)},

    // back of east-facing
    {phx::math::vec3( 1.75f,  1.75f,  1.75f), phx::math::vec2(1.f, 1.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3( 1.75f, -1.75f,  1.75f), phx::math::vec2(1.f, 0.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.75f, -1.75f, -1.75f), phx::math::vec2(0.f, 0.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.75f, -1.75f, -1.75f), phx::math::vec2(0.f, 0.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.75f,  1.75f, -1.75f), phx::math::vec2(0.f, 1.f), phx::math::vec3(1.f, 0.f, 1.f)},
    {phx::math::vec3( 1.75f,  1.75f,  1.75f), phx::math::vec2(1.f, 1.f), phx::math::vec3(1.f, 0.f, 1.f)},

    // west-facing front
    {phx::math::vec3(-1.75f, -1.75f,  1.75f), phx::math::vec2(1.f, 0.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.75f, -1.75f, -1.75f), phx::math::vec2(0.f, 0.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.75f,  1.75f, -1.75f), phx::math::vec2(0.f, 1.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3( 1.75f,  1.75f, -1.75f), phx::math::vec2(0.f, 1.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3(-1.75f,  1.75f,  1.75f), phx::math::vec2(1.f, 1.f), phx::math::vec3(1.f, 0.f, -1.f)},
    {phx::math::vec3(-1.75f, -1.75f,  1.75f), phx::math::vec2(1.f, 0.f), phx::math::vec3(1.f, 0.f, -1.f)},

    // back of west-facing
    {phx::math::vec3( 1.75f,  1.75f, -1.75f), phx::math::vec2(1.f, 1.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3( 1.75f, -1.75f, -1.75f), phx::math::vec2(1.f, 0.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.75f, -1.75f,  1.75f), phx::math::vec2(0.f, 0.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.75f, -1.75f,  1.75f), phx::math::vec2(0.f, 0.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3(-1.75f,  1.75f,  1.75f), phx::math::vec2(0.f, 1.f), phx::math::vec3(-1.f, 0.f, 1.f)},
    {phx::math::vec3( 1.75f,  1.75f, -1.75f), phx::math::vec2(1.f, 1.f), phx::math::vec3(-1.f, 0.f, 1.f)},

	// front (north)
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, -1)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 0, -1)},

	// left (west)
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(-1, 0, 0)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(-1, 0, 0)},

	// back (south)
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 0, 1)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 0, 1)},

	// right (east)
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(1, 0, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(1, 0, 0)},

	// top
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f,  1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f,  1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f,  1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, 0)},

	// bottom
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f, -1.f, -1.f), phx::math::vec2(1.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f, -1.f, -1.f), phx::math::vec2(0.f,  0.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3(-1.f, -1.f,  1.f), phx::math::vec2(0.f,  1.f), phx::math::vec3(0, 1, 0)},
    {phx::math::vec3( 1.f, -1.f,  1.f), phx::math::vec2(1.f,  1.f), phx::math::vec3(0, 1, 0)},
};

static constexpr const phx::gfx::DefaultMeshVertex* XPANEL_BLOCK_MESH             = &vertXPanBlock[0];
static constexpr std::size_t                        XPANEL_BLOCK_PANEL_VERT_COUNT = 24;
static constexpr const phx::gfx::DefaultMeshVertex* XPANEL_BLOCK_BLOCK_MESH       = &vertXPanBlock[24];
static constexpr std::size_t                        XPANEL_BLOCK_BLOCK_VERT_COUNT = 36;
static constexpr std::size_t                        XPANEL_BLOCK_MAX_VERTS        = 60;
// clang-format on
