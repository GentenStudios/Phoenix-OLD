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

#include <Common/CoreIntrinsics.hpp>

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <sstream>

#ifdef ENGINE_DEBUG
#	define GLCheck(x) \
		x;             \
		phx::gfx::checkError(__FILE__, __LINE__);
#else
#	define GLCheck(x) x;
#endif

#ifndef ENGINE_PLATFORM_WINDOWS
#	define __stdcall
#endif

// Uncomment this in order to enable detailed OpenGL logging.
#define OPENGL_DIAGNOSTICS_ENABLED

namespace phx::gfx
{
	inline void checkError(const char* file, const int line)
	{
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (errorCode)
			{
			case GL_INVALID_ENUM:
				error = "INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				error = "INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				error = "INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				error = "STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				error = "STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				error = "OUT_OF_MEMORY";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error = "INVALID_FRAMEBUFFER_OPERATION";
				break;
			default:
				error = "UNKNOWN";
				break; // Something is seriously wrong, get this
				       // fixed.
			}

			std::cout << "[WARNING] - " << file << ":" << line
			          << " - [RENDERING][OPENGL ERROR] " << error << std::endl;
		}
	}

	/**
	 * @brief OpenGL Debugging output function. THIS FUNCTION IS
	 * HANDLED BY OPENGL, AND DOES NOT NEED *ANY* INTERVENTION FROM
	 * PROGRAMMERS.
	 *
	 * @param source The source of the error, whether it be
	 * an API error, or WINDOW SYSTEM error, or maybe even a THIRD
	 * PARTY error.
	 *
	 * @param type The type of error, as in ERROR, or
	 * DEPRECATED BEHAVIOUR, or other types of errors that OpenGL
	 * recognizes.
	 *
	 * @param id The ID of the error, this is often very
	 * vague and can be quite useless.
	 *
	 * @param severity The severity of the error, whether it be
	 *
	 * HIGH, MEDIUM, or LOW severity, or even just a notification.
	 * @param length The length of the message. (not used by
	 * us.)
	 *
	 * @param message The message itself.
	 *
	 * @param userParam Any Parameters we decide to make OpenGL
	 * send.
	 *
	 * ALL THESE PARAMETERS ARE SET BY OPENGL, AND ALL FUNCTION
	 * CALLS ARE DONE BY OPENGL, DO NOT INTERFERE WITH THIS, OR YOU
	 * MAY BREAK DEBUGGING OF OPENGL.
	 */
	inline void __stdcall glDebugOutput(GLenum source, GLenum type, GLuint id,
	                                    GLenum severity, GLsizei length,
	                                    const GLchar* message,
	                                    const void*   userParam)
	{

#ifdef OPENGL_DIAGNOSTICS_ENABLED
		(void) id;
		(void) length;
		(void) userParam;

		std::stringstream subCategories;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			subCategories << "[HIGH SEVERITY]";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			subCategories << "[MEDIUM SEVERITY]";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			subCategories << "[LOW SEVERITY]";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			subCategories << "[NOTIFICATION]";
			break;
		default:
			subCategories << "[UNKNOWN]";
			break; // Something is seriously wrong.
		}

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			subCategories << "[API]";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			subCategories << "[WINDOW SYSTEM]";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			subCategories << "[SHADER COMPILER]";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			subCategories << "[THIRD PARTY]";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			subCategories << "[APPLICATION]";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			subCategories << "[OTHER]";
			break;
		default:
			subCategories << "[UNKNOWN]";
			break; // Something is seriously wrong.
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			subCategories << "[ERROR]";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			subCategories << "[DEPRECATED BEHAVIOUR]";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			subCategories << "[UNDEFINED BEHAVIOUR]";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			subCategories << "[PORTABILITY]";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			subCategories << "[PERFORMANCE]";
			break;
		case GL_DEBUG_TYPE_MARKER:
			subCategories << "[MARKER]";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			subCategories << "[PUSH GROUP]";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			subCategories << "[POP GROUP]";
			break;
		case GL_DEBUG_TYPE_OTHER:
			subCategories << "[OTHER]";
			break;
		default:
			subCategories << "[UNKNOWN]";
			break; // Something is seriously wrong.
		}

		subCategories << " ";

		std::cout << "[RENDERING]" << subCategories.str() << " " << message
		          << std::endl;
#endif
	}
} // namespace phx::gfx

