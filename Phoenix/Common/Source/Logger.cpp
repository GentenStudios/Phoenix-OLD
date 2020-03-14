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

#include <Common/CoreIntrinsics.hpp>
#include <Common/Logger.hpp>

#ifdef ENGINE_PLATFORM_WINDOWS
#	include <Windows.h>
#endif

#include <cstdio>

using namespace phx;

static const char* g_logVerbToText[] = {"FATAL", "WARNING", "INFO", "DEBUG"};

#ifdef ENGINE_PLATFORM_WINDOWS
static WORD s_windowsTermCol[] = {
    FOREGROUND_INTENSITY | FOREGROUND_RED,                    // Red
    FOREGROUND_INTENSITY | FOREGROUND_GREEN,                  // Green
    FOREGROUND_INTENSITY | FOREGROUND_BLUE,                   // Blue
    FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN, // Yellow
    FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |
        FOREGROUND_BLUE, // White
};
#endif

#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_APPLE)
static const char* s_linuxTermCol[] = {
    "\033[1;31m", // Red
    "\033[1;32m", // Green
    "\033[1;34m", // Blue
    "\033[0;33m", // Yellow
    "\033[1;37m"  // White
};
#endif

enum class TextColor
{
	RED    = 0,
	GREEN  = 1,
	BLUE   = 2,
	YELLOW = 3,
	WHITE  = 4,
};

static void setTerminalTextColor(TextColor color)
{
#ifdef ENGINE_PLATFORM_WINDOWS
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console,
	                        s_windowsTermCol[static_cast<std::size_t>(color)]);
#endif

#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_APPLE)
	printf("%s", s_linuxTermCol[static_cast<std::size_t>(color)]);
#endif
}

static void setTerminalTextColor(LogVerbosity vb)
{
	TextColor color;

	switch (vb)
	{
	case LogVerbosity::FATAL:
		color = TextColor::RED;
		break;
	case LogVerbosity::WARNING:
		color = TextColor::YELLOW;
		break;
	case LogVerbosity::INFO:
		color = TextColor::WHITE;
		break;
	case LogVerbosity::DEBUG:
		color = TextColor::GREEN;
		break;
	default:
		color = TextColor::WHITE;
		break;
	}

	setTerminalTextColor(color);
}

Logger* Logger::m_instance = nullptr;

Log::Log(LogVerbosity vb, const std::string& errFile, int errLineNo,
         const std::string& module)
{
	verbosity = vb;
	errorFile = errFile;
	errorLine = errLineNo;
	component = module;
}

Log::Log(const Log& rhs)
{
	stream    = std::stringstream(rhs.stream.str());
	verbosity = rhs.verbosity;
	errorFile = rhs.errorFile;
	errorLine = rhs.errorLine;
	component = rhs.component;
}

Log& Log::operator=(const Log& rhs)
{
	stream    = std::stringstream(rhs.stream.str());
	verbosity = rhs.verbosity;
	errorFile = rhs.errorFile;
	errorLine = rhs.errorLine;
	component = rhs.component;

	return *this;
}

void Logger::initialize(const LoggerConfig& config)
{
	m_instance = new Logger(config);
}

void Logger::teardown() { delete m_instance; }

LogVerbosity Logger::getVerbosity() const { return m_verbosity; }

void Logger::log(const Log& log)
{
	if (log.verbosity > m_verbosity)
	{
		return;
	}

	if (!m_threaded || log.verbosity == LogVerbosity::FATAL)
	{
		loggerInternal(log);
		return;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	m_messages.push_back(log);
}

void Logger::operator+=(const Log& stream) { log(stream); }

Logger::Logger(const LoggerConfig& config)
{
	m_threaded     = config.threaded;
	m_verbosity    = config.verbosity;
	m_logToConsole = config.logToConsole;

	if (m_threaded)
	{
		m_threadRunning    = true;
		std::thread thread = std::thread(&Logger::loggerThreadHandle, this);
		m_worker.swap(thread);
	}

	m_file.open(config.logFile);
	if (!m_file.is_open())
	{
		printf("Uh Oh! We couldn't open the log file, guess we won't have any "
		       "file logging for today. :(\n");
	}
}

Logger::~Logger()
{
	printf("\n");

	if (m_threaded)
	{
		m_threadRunning = false;
		m_cond.notify_all();

		if (m_worker.joinable())
			m_worker.join();
	}
}

void Logger::loggerInternal(const Log& log)
{
	if (m_logToConsole)
	{
		if (log.verbosity == LogVerbosity::INFO)
		{
			setTerminalTextColor(LogVerbosity::INFO);
			printf("[%s][%s] %s\n",
			       g_logVerbToText[static_cast<int>(log.verbosity)],
			       log.component.c_str(), log.stream.str().c_str());
			setTerminalTextColor(TextColor::WHITE);
		}
		else
		{
			setTerminalTextColor(log.verbosity);
#ifdef ENGINE_DEBUG
			printf("[%s] %s:%i [%s] %s\n",
			       g_logVerbToText[static_cast<int>(log.verbosity)],
			       log.errorFile.c_str(), log.errorLine, log.component.c_str(),
			       log.stream.str().c_str());
			setTerminalTextColor(TextColor::WHITE);
#else
			printf("[%s][%s] %s\n",
			       g_logVerbToText[static_cast<int>(log.verbosity)],
			       log.component.c_str(), log.stream.str().c_str());
			setTerminalTextColor(TextColor::WHITE);
#endif
		}
	}
}

void Logger::loggerThreadHandle()
{
	while (true)
	{
		Log log;

		{
			std::unique_lock<std::mutex> lock(m_mutex);

			m_cond.wait(lock, [this] {
				return !m_threadRunning || !m_messages.empty();
			});

			if (!m_threadRunning && m_messages.empty())
			{
				return;
			}

			log = m_messages.front();
			m_messages.pop_front();
		}

		loggerInternal(log);
	}
}
