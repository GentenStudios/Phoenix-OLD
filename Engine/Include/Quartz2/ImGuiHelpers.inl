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

// The headers are defined in ImGuiHelpers.hpp

namespace ImGui
{
	inline BaseImWindow::BaseImWindow(const char* name)
	{
		rootWindowName = name;
	};

	inline void BaseImWindow::begin(bool* p_open, ImGuiWindowFlags flags)
	{
		ImGui::Begin(rootWindowName, p_open, flags);
	}
	inline void BaseImWindow::end()
	{
		ImGui::End();
	};

	inline void BaseImWindow::draw()
	{
		drawEx(NULL, ImGuiWindowFlags_None);
	};
	inline void BaseImWindow::draw(bool* p_open)
	{
		drawEx(p_open, ImGuiWindowFlags_None);
	};
	inline void BaseImWindow::draw(bool* p_open, ImGuiWindowFlags flags)
	{
		drawEx(p_open, flags);
	};

	// NOTE: this must at least be defined otherwise the linker freaks.
	inline void BaseImWindow::drawEx(bool* p_open, ImGuiWindowFlags flags)
	{
			/*TODO: throw error must override */
	};



	inline BasicTerminal::BasicTerminal(const char* name, int outputKiloBytes,
	                                    const char* initialContents)
			: BaseImWindow(name)
	{
		outputWindowName = std::string(name).append("_Output").c_str();
		targetOutputSize = outputKiloBytes * 1024; // align to kilobytes
		inputBuffer      = std::string("");
		cache            = initialContents;
		cache.reserve(targetOutputSize);
		cout = std::ostringstream(std::ios_base::out | std::ios_base::ate);
	};
	inline BasicTerminal::BasicTerminal(const char* name, int outputKiloBytes)
			: BaseImWindow(name)
	{
		outputWindowName = std::string(name).append("_Output").c_str();
		targetOutputSize = outputKiloBytes * 1024; // align to kilobytes
		inputBuffer      = std::string("");
		cache            = std::string("");
		cache.reserve(targetOutputSize);
		cout =
				std::ostringstream(std::ios_base::out | std::ios_base::ate);
	};

	inline BasicTerminal::~BasicTerminal()
	{
		// TODO: Possibly deconstruct the strings, I know I'm missing something.
	};

	inline void BasicTerminal::drawEx(bool* p_open, ImGuiWindowFlags extra_flags)
	{
		// Window Definition (remember to use the terminal class' begin)
		begin(p_open, defaultFlags | extra_flags);
		drawOutputField(ImGuiWindowFlags_None);
		drawInputField();
		end();
	}

	inline void BasicTerminal::registerCallback(TerminalCallback callback)
	{
		callbackRegistry.push_back(callback);
	}
};
