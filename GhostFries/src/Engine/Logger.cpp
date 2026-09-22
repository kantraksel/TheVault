#include <chrono>
#include <functional>
#include <fstream>
#include <iostream>
#include <mutex>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif
#define LOGGER_ENABLE_CALLBACK
#include "Engine/Logger.h"
#include "Utility/StringUtils.h"

enum Color : char
{
	Black = '0',
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
	RGB,
};

struct ColorEx
{
	Color code;
	unsigned char r, g, b;
};

using LogLevel = Logger::LogLevel;

struct LoggerImpl
{
	std::ofstream file;
	std::mutex mutex;
	std::function<void(const std::string_view&, LogLevel level)> logCallback;
	bool timestampEnabled;
	bool consoleOut;

	LoggerImpl();
	void LogRaw(const std::string_view&, ColorEx, LogLevel);
	static void LogConsole(const std::string_view&, ColorEx);

#if _WIN32
	void OpenLogFile(const std::wstring&);
#else
	void OpenLogFile(const std::string&);
#endif
	void LogFormat(const std::string_view&, LogLevel);
	void Log(const std::string_view&, ColorEx, LogLevel);
};

alignas(LoggerImpl) static unsigned char buffer[sizeof(LoggerImpl)]{};
static LoggerImpl* pInstance = nullptr;

#if _DEBUG
static bool loggerDestructing = false;
#endif

struct LoggerInit
{
	LoggerInit()
	{
		Construct();
	}

	~LoggerInit()
	{
	#if _DEBUG
		loggerDestructing = true;
	#endif
		if (!pInstance)
			return;

		pInstance->~LoggerImpl();
		pInstance = nullptr;
	}

	static void Construct()
	{
		if (pInstance)
			return;
	#if _DEBUG
		if (loggerDestructing)
			LoggerImpl::LogConsole("Logger should not be used in destructors", { Color::RGB, 229, 41, 255 });
	#endif
		
		pInstance = new(buffer) LoggerImpl;
	}
};
static LoggerInit init;

LoggerImpl::LoggerImpl()
{
	timestampEnabled = true;
	consoleOut = true;

#if _WIN32
	HANDLE hOut;
#if _DEBUG
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == NULL && AllocConsole())
	{
		FILE* fDummy;
		freopen_s(&fDummy, "CONIN$", "r", stdin);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
	}
#endif
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	DWORD mode = 0;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hOut, &mode);
	SetConsoleMode(hOut, mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	consoleOut = hOut != NULL;

	std::cin.clear();
	std::cout.clear();
	std::cerr.clear();
#endif
}

#if _WIN32
void LoggerImpl::OpenLogFile(const std::wstring& name)
#else
void LoggerImpl::OpenLogFile(const std::string& name)
#endif
{
	std::lock_guard lock(mutex);

	file.close();
	file.open(name, std::ofstream::app);

	auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	auto out = std::format("Starting logger - {:%c}", time);
	LogRaw(out, { Color::Green }, LogLevel::Custom);
}

void LoggerImpl::LogRaw(const std::string_view& out, ColorEx fontColor, LogLevel level)
{
	if (consoleOut)
		LogConsole(out, fontColor);

	if (file)
	{
		file << out << std::endl;
		file.flush();
	}

	if (logCallback)
		logCallback(out, level);
}

static void PrintColor(char flag, ColorEx color)
{
	std::cout << flag << color.code;
	if (color.code == Color::RGB)
	{
		std::cout << ";2;";
		std::cout << std::to_string(color.r);
		std::cout << ';';
		std::cout << std::to_string(color.g);
		std::cout << ';';
		std::cout << std::to_string(color.b);
	}
}

void LoggerImpl::LogConsole(const std::string_view& out, ColorEx fontColor)
{
	std::cout << "\x1b[";
	PrintColor('3', fontColor);
	std::cout << ';';
	PrintColor('4', { Color::Black });
	std::cout << 'm';
	std::cout << out << std::endl;
}

static const char* GetColor(LogLevel level, Color& color)
{
	switch (level)
	{
		case LogLevel::Debug:
		{
			color = Color::Cyan;
			return "";
		}
		case LogLevel::Info:
		{
			color = Color::White;
			return "";
		}
		case LogLevel::Warning:
		{
			color = Color::Yellow;
			return "[Warning] ";
		}
		case LogLevel::Error:
		{
			color = Color::Red;
			return "[ERROR] ";
		}
		default:
		{
			color = Color::White;
			return "";
		}
	}
}

void LoggerImpl::LogFormat(const std::string_view& str, LogLevel level)
{
	Color color;
	auto* levelPrefix = GetColor(level, color);

	std::string out;
	if (timestampEnabled)
	{
		auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
		out = std::format("[{:%X}] {}{}", time, levelPrefix, str.data());
	}
	else
		out = std::format("{}{}", levelPrefix, str.data());

	Log(out, { color }, level);
}

void LoggerImpl::Log(const std::string_view& str, ColorEx fontColor, LogLevel level)
{
	std::lock_guard lock(mutex);
	LogRaw(str, fontColor, level);
}

static void Log(const std::string_view& str, LogLevel level)
{
	LoggerInit::Construct();
	pInstance->LogFormat(str, level);
}

static void LogEx(const std::string_view& str, Logger::RGB rgb, LogLevel level)
{
	LoggerInit::Construct();
	pInstance->Log(str, { Color::RGB, rgb.r, rgb.g, rgb.b }, level);
}

// public Logger
#if _WIN32
void Logger::OpenLogFile(const std::wstring& name)
#else 
void Logger::OpenLogFile(const std::string& name)
#endif
{
	LoggerInit::Construct();
	pInstance->OpenLogFile(name);
}

bool Logger::IsLogOpen()
{
	LoggerInit::Construct();
	return (bool)pInstance->file;
}

void Logger::SetTitle(const std::string_view& title)
{
	std::cout << "\x1b]0;" << title << "\x1b\x5c";
}

void Logger::SetTimestamp(bool value)
{
	LoggerInit::Construct();
	pInstance->timestampEnabled = value;
}

void Logger::SetConsoleOut(bool value)
{
	LoggerInit::Construct();
	pInstance->consoleOut = value;
}

void Logger::Log(const std::string_view& str)
{
	::Log(str, LogLevel::Info);
}

void Logger::Log(const std::wstring_view& str)
{
	::Log(StringUtils::WideStringToUtf8(str), LogLevel::Info);
}

void Logger::LogWarn(const std::string_view& str)
{
	::Log(str, LogLevel::Warning);
}

void Logger::LogWarn(const std::wstring_view& str)
{
	::Log(StringUtils::WideStringToUtf8(str), LogLevel::Warning);
}

void Logger::LogError(const std::string_view& str)
{
	::Log(str, LogLevel::Error);
}

void Logger::LogError(const std::wstring_view& str)
{
	::Log(StringUtils::WideStringToUtf8(str), LogLevel::Error);
}

#if _DEBUG
void Logger::LogDebug(const std::string_view& str)
{
	::Log(str, LogLevel::Debug);
}

void Logger::LogDebug(const std::wstring_view& str)
{
	::Log(StringUtils::WideStringToUtf8(str), LogLevel::Debug);
}
#endif

void Logger::LogEx(const std::string_view& str, RGB rgb, LogLevel level)
{
	::LogEx(str, rgb, level);
}

void Logger::LogEx(const std::wstring_view& str, RGB rgb, LogLevel level)
{
	::LogEx(StringUtils::WideStringToUtf8(str), rgb, level);
}

Logger::Callback Logger::SetLogCallback(const Callback& callback)
{
	LoggerInit::Construct();
	auto old = pInstance->logCallback;
	pInstance->logCallback = callback;
	return old;
}
