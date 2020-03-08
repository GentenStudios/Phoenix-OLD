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

#include <condition_variable>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace phx
{
	enum class LogVerbosity : int
	{
		FATAL   = 0,
		WARNING = 1,
		INFO    = 2,
		DEBUG   = 3,
	};

	struct LoggerConfig
	{
		std::string  logFile;
		LogVerbosity verbosity         = LogVerbosity::INFO;
		bool         threaded          = false;
		bool         includeTimestamps = false;
		bool         logToConsole      = true;
	};

	class Log
	{
	public:
		Log(LogVerbosity vb, const std::string& errFile, int errLineNo, const std::string& module)
		{
			verbosity = vb;
			errorFile = errFile;
			errorLine = errLineNo;
			component = component;
		}

		Log& ref() { return *this; }

		template <typename T>
		Log& operator<<(T& msg)
		{
			stream << msg;
			return *this;
		}

		std::stringstream stream;
		LogVerbosity      verbosity;
		std::string       errorFile;
		int               errorLine;
		std::string       component;
	};

	class Logger
	{
	public:
		static void initialize(const LoggerConfig& config);
		void        teardown();

		static Logger* get() { return m_instance; }

		void log(const Log& log);

		void operator+=(const Log& stream);

	private:
		Logger(const LoggerConfig& config);
		~Logger() = default;

		void loggerInternal(const Log& log);
		void loggerInternalForThreading();

		static Logger* m_instance;

	private:
		std::vector<std::string> m_messages;

		LogVerbosity m_verbosity;

		bool                    m_threaded;
		std::condition_variable m_cond;
		std::mutex              m_mutex;

		std::ofstream m_file;
	};
} // namespace phx
