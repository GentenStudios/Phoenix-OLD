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

#include <Client/Graphics/OpenGLTools.hpp>
#include <Client/Graphics/SkyboxRenderer.hpp>

#include <Common/Logger.hpp>

#include <glad/glad.h>
#include <stb_image.h>

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

using namespace phx::gfx;

void SkyboxRenderer::setSkyboxTextures(const std::vector<std::string>& textures)
{
	// we want the order front, left, back, right, top, bottom
	if (textures.size() == 6)
	{
		m_enabled = true;
		
		int width[6] = {};
		int height[6] = {};
		int channels[6] = {};

		for (std::size_t i = 0; i < 6; ++i)
		{
			stbi_info(textures[i].c_str(), &width[i], &height[i], &channels[i]);

			if (width[i] != height[i])
			{
				LOG_WARNING("WORLD RENDERER")
				    << "The skybox texture: " << textures[i]
				    << " is not a square. No skybox will be used.";

				m_enabled = false;
				return;
			}

			// this will make sure all the files are equal in dimensions.
			if (width[i] != width[0])
			{
				LOG_WARNING("WORLD RENDERER")
				    << "The skybox texture: " << textures[i]
				    << " does not match the resolution: " << width[i] << "x"
				    << height[i]
				    << " set by the initially loaded skybox texture.";

				m_enabled = false;
                return;
			}
		}

		GLCheck(glGenTextures(1, &m_skyboxTex));
		GLCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTex));

		stbi_set_flip_vertically_on_load(false);
		auto loadData = [this](const std::string& tex, GLenum cubemapLoc)
		{
			if (m_enabled)
			{
				int            x, y, chan;
				unsigned char* data = stbi_load(tex.c_str(), &x, &y, &chan, 0);
				if (data == nullptr)
				{
					LOG_WARNING("WORLD RENDERER")
					    << "The texture: " << tex
					    << " could not be loaded correctly.";

					m_enabled = false;
				}
				else
				{
					GLCheck(glTexImage2D(cubemapLoc, 0, GL_RGBA, x, y, 0,
					             (chan == 4) ? GL_RGBA : GL_RGB,
					             GL_UNSIGNED_BYTE, data));
				}

				stbi_image_free(data);
			}
		};
		
		// front, left, back, right, top, bottom
		loadData(textures[0], GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		loadData(textures[1], GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		loadData(textures[2], GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		loadData(textures[3], GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		loadData(textures[4], GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		loadData(textures[5], GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		
		GLCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCheck(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	}
	else
	{
		LOG_WARNING("WORLD RENDERER") << "6 skybox textures were expected, "
		                              << textures.size() << " were given.";
	}
}

void SkyboxRenderer::tick(
    entt::registry*   registry,
    entt::entity      entity,
    const math::mat4& projection,
    const float&      dt)
{
    auto position = registry->get<Position>(entity);
	if (m_initialTick)
	{
		if (m_enabled)
		{
			glGenVertexArrays(1, &m_skyboxVao);
			glBindVertexArray(m_skyboxVao);

			glGenBuffers(1, &m_skyboxVbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVbo);
			glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float),
			             &skyboxVertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			                      nullptr);

			m_skyboxPipeline.prepare("Assets/Skybox.vert", "Assets/Skybox.frag",
			                         {{"a_Pos", 0}});

			m_skyboxPipeline.activate();
			m_skyboxPipeline.setInt("u_Tex", 0);
		}

		m_initialTick = false;
	}

	// skybox should be rendered last.
	if (m_enabled)
	{
		glBindVertexArray(m_skyboxVao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTex);

		m_skyboxPipeline.activate();

		// @todo prevent calculating view matrix like 5 times a frame for no
		// reason. implement cache.

		auto view = position.getView();
		view.elements[0 + 3 * 4] = 0;
		view.elements[1 + 3 * 4] = 0;
		view.elements[2 + 3 * 4] = 0;
		view.elements[3 + 3 * 4] = 0;
		view.elements[3 + 0 * 4] = 0;
		view.elements[3 + 1 * 4] = 0;
		view.elements[3 + 2 * 4] = 0;
		
		m_skyboxPipeline.setMatrix("u_view", view);
		m_skyboxPipeline.setMatrix("u_projection", projection);

		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
	}
}
