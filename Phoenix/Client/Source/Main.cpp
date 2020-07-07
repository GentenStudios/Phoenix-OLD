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

#include <Client/Client.hpp>
#include <Client/Graphics/ShaderPipeline.hpp>
#include <Client/Graphics/TexturePacker.hpp>
#include <Client/Graphics/Window.hpp>

#include <Common/Logger.hpp>

#include <glad/glad.h>

#include <filesystem>
namespace fs = std::filesystem;

using namespace phx;

#undef main
int main(int argc, char** argv)
{
	// client::Client::get()->run();

	Logger::initialize({});

	gfx::Window window("yoyo waddup g", 1280, 720);

	gfx::TexturePacker packer;

	const std::string path = "Modules/mod1/TESTING/";
	for (const auto& entry : fs::directory_iterator(path))
	{
		packer.add(path + entry.path().filename().string());
	}

	packer.pack();

	math::vec2 vertices[] = {
	    // positions
	    {0.5f, 0.5f},
	    {0.5f, -0.5f},
	    {-0.5f, -0.5f},
	    {-0.5f, 0.5f},
	};

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	const gfx::TextureData* uvs = packer.getData(30);

	std::vector<math::vec2> verts;
	verts.push_back(vertices[0]);
	verts.push_back(uvs->bottomLeftUV + uvs->uvSize);
	verts.push_back(vertices[1]);
	verts.push_back({uvs->bottomLeftUV.x + uvs->uvSize.x, uvs->bottomLeftUV.y});
	verts.push_back(vertices[3]);
	verts.push_back({uvs->bottomLeftUV.x, uvs->bottomLeftUV.y + uvs->uvSize.y});
	verts.push_back(vertices[1]);
	verts.push_back({uvs->bottomLeftUV.x + uvs->uvSize.x, uvs->bottomLeftUV.y});
	verts.push_back(vertices[2]);
	verts.push_back(uvs->bottomLeftUV);
	verts.push_back(vertices[3]);
	verts.push_back({uvs->bottomLeftUV.x, uvs->bottomLeftUV.y + uvs->uvSize.y});

	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(math::vec2),
	             verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(math::vec2),
	                      nullptr);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(math::vec2),
	                      reinterpret_cast<void*>(sizeof(math::vec2)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	std::vector<gfx::ShaderLayout> layout;
	layout.emplace_back("a_Vert", 0);
	layout.emplace_back("a_UV", 1);

	gfx::ShaderPipeline pipeline;
	pipeline.prepare("Assets/Test.vert", "Assets/Test.frag", layout);

	pipeline.activate();
	packer.activate(0);
	pipeline.setFloat("u_Layer", uvs->layer);

	while (window.isRunning())
	{
		window.startFrame();

		glDrawArrays(GL_TRIANGLES, 0, 6);

		window.endFrame();
	}

	return 0;
}
