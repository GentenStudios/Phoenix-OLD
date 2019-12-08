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

#if defined(_MSC_VER)
	// Make MSVC shut up about using fopen_s instead of fopen.
	// #todo (bdw): This is probably good advice & we should listen
	//              FIX - Change fopen to fopen_s in compileShader below.
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Quartz2/ShaderCompiler.hpp>

#include <cstdio>

using namespace q2;

namespace
{
	static std::optional<GLuint> compileShader(const std::string& filepath, GLenum type)
	{
		FILE* file = fopen(filepath.c_str(), "r");
		if (!file)
			return {};

		fseek(file, 0, SEEK_END);
		const std::size_t fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		char* fileBuffer = new char[fileSize + 1];

		const std::size_t nRead = fread(fileBuffer, sizeof(char), fileSize, file);
		fileBuffer[nRead] = '\0';

		const GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &fileBuffer, NULL);
	
		delete[] fileBuffer;
		fclose(file);

		glCompileShader(shader);

		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			const std::size_t INFO_LOG_SIZE = 512;
			char infoLog[INFO_LOG_SIZE];
			glGetShaderInfoLog(shader, INFO_LOG_SIZE, NULL, infoLog);

			std::fprintf(stderr, "%s compile error:\n%s",
				type == GL_VERTEX_SHADER ? "Vertex" : type == GL_FRAGMENT_SHADER ? "Fragment" : NULL,
				infoLog);
			
			return {};
		}

		return shader;
	}
}

std::optional<GLuint> ShaderCompiler::compile(const std::string& vertexShader, const std::string& fragmentShader)
{
	const std::optional<GLuint> vertexShaderId = compileShader(vertexShader, GL_VERTEX_SHADER);
	const std::optional<GLuint> pixelShderId = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

	if (!vertexShaderId.has_value() || !pixelShderId.has_value())
		return {};

	const GLuint program = glCreateProgram();
	
	glAttachShader(program, vertexShaderId.value());
	glAttachShader(program, pixelShderId.value());

	glLinkProgram(program);

	glDeleteShader(vertexShaderId.value());
	glDeleteShader(pixelShderId.value());

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		const std::size_t INFO_LOG_SIZE = 512;
		char infoLog[INFO_LOG_SIZE];

		glGetProgramInfoLog(program, INFO_LOG_SIZE, NULL, infoLog);
		std::fprintf(stderr, "Shader link error:\n%s", infoLog);
	
		return {};
	}

	return program;
}