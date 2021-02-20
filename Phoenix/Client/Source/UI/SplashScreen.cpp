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

#include <Client/UI/SplashScreen.hpp>

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>

using namespace phx::client;
using namespace phx;

SplashScreen::SplashScreen() : Layer("SplashScreen") {}

void SplashScreen::onEvent(events::Event& e) {}

void SplashScreen::onAttach()
{
	// verts and uvs packed.
	float vertices[] = {
	    0.25f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
	    0.25f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
	    -0.25f, 0.5f,  0.0f, 0.0f, 1.0f, // top left
	    0.25f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
	    -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
	    -0.25f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
	};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
	                      reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	int            width, height, nrChannels;
	unsigned char* data =
	    stbi_load("Assets/GSLogo.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		             GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	stbi_set_flip_vertically_on_load(false);

	std::vector<gfx::ShaderLayout> layout;
	layout.emplace_back("a_Vertex", 0);
	layout.emplace_back("a_UV", 1);

	m_pipeline.prepare("Assets/Splashscreen.vert", "Assets/Splashscreen.frag",
	                   layout);
}

void SplashScreen::onDetach()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteTextures(1, &m_texture);
	glDeleteVertexArrays(1, &m_vao);
}

void SplashScreen::tick(float dt)
{
	m_pipeline.activate();

	glBindVertexArray(m_vao);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	m_alpha += dt * m_multiplier;
	if (m_alpha >= 2.f) // 1 second of fading in, 1 second of wait.
	{
		signalRemoval();
	}

	m_pipeline.setFloat("u_TexAlpha", m_alpha);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

