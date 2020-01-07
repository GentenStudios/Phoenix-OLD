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

#include <Quartz2/Math/Math.hpp>

#include <glad/glad.h>

#include <string>
#include <vector>

namespace q2
{
	namespace gfx
	{
		struct ShaderLayout
		{
			ShaderLayout(std::string attribName, int desiredIndex)
			    : attribName(attribName), desiredIndex(desiredIndex)
			{}

			std::string attribName;
			int desiredIndex = -1;
		};

		class ShaderPipeline
		{
		public:
			ShaderPipeline()  = default;
			~ShaderPipeline() = default;

			void prepare(std::string vertShader, std::string fragShader, std::vector<ShaderLayout> layout);
			void activate();

			void setVector2(std::string location, math::vec2 value);
			void setVector3(std::string location, math::vec3 value);
			void setMatrix(std::string location, math::mat4 value);

			int queryLayoutOfAttribute(std::string attr);

		private:
			unsigned int m_program;
		};
	} // namespace gfx
} // namespace q2
