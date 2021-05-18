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
#include <Client/Graphics/SelectionboxRenderer.hpp>

using namespace phx::gfx;

SelectionboxRenderer::SelectionboxRenderer()
{
    glGenVertexArrays(1, &m_selectionBoxVAO);
    glBindVertexArray(m_selectionBoxVAO);
    glGenBuffers(1, &m_selectionBoxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_selectionBoxVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    m_selectionBoxPipeline.prepare("Assets/SimpleLines.vert",
    "Assets/SimpleLines.frag", {{"a_Pos", 0}});
}

void SelectionboxRenderer::tick(
    const Position& position,
    const math::mat4& projection,
    math::vec3 selection)
{
    // do not waste cpu time if we aren't targeting a solid block
    if (position.map->getBlockAt(selection)->category !=
        voxels::BlockCategory::SOLID)
    {
        return;
    }

    // voxel position to camera position
	selection.x = (selection.x - 0.5f) * 2.f;
	selection.y = (selection.y - 0.5f) * 2.f;
	selection.z = (selection.z - 0.5f) * 2.f;

    /*
           1 +--------+ 2
            /|       /|
           / |   3  / |
        0 +--------+  |
          |  |6    |  |
          |  x-----|--+ 7
          | /      | /
          |/       |/
        5 +--------+ 4
     */

    const float more = 2.001f;
    const float less = 0.001f;

    float vertices[] = {
	    selection.x + more, selection.y + more, selection.z - less, // 0-1
	    selection.x - less, selection.y + more, selection.z - less,

	    selection.x - less, selection.y + more, selection.z - less, // 1-2
	    selection.x - less, selection.y + more, selection.z + more,

	    selection.x - less, selection.y + more, selection.z + more, // 2-3
	    selection.x + more, selection.y + more, selection.z + more,

	    selection.x + more, selection.y + more, selection.z + more, // 3-4
	    selection.x + more, selection.y - less, selection.z + more,

	    selection.x + more, selection.y - less, selection.z + more, // 4-5
	    selection.x + more, selection.y - less, selection.z - less,

	    selection.x + more, selection.y - less, selection.z - less, // 5-6
	    selection.x - less, selection.y - less, selection.z - less,

	    selection.x - less, selection.y - less, selection.z - less, // 6-7
	    selection.x - less, selection.y - less, selection.z + more,

	    selection.x - less, selection.y - less, selection.z + more, // 7-4
	    selection.x + more, selection.y - less, selection.z + more,

	    selection.x - less, selection.y - less, selection.z + more, // 7-2
	    selection.x - less, selection.y + more, selection.z + more,

	    selection.x - less, selection.y + more, selection.z - less, // 1-6
	    selection.x - less, selection.y - less, selection.z - less,

	    selection.x + more, selection.y + more, selection.z - less, // 0-3
	    selection.x + more, selection.y + more, selection.z + more,

	    selection.x + more, selection.y + more, selection.z - less, // 0-5
	    selection.x + more, selection.y - less, selection.z - less};

    glBindVertexArray(m_selectionBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_selectionBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    m_selectionBoxPipeline.activate();

    // @todo fix so we aren't calculating the same thing twice (once in the main
    // render loop and once more here).
    m_selectionBoxPipeline.setMatrix("u_view", position.getView());
    m_selectionBoxPipeline.setMatrix("u_projection", projection);
    glDrawArrays(GL_LINES, 0, 24);
}