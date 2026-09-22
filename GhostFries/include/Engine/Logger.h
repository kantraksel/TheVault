#pragma once
#include <format>
#ifdef LOGGER_ENABLE_CALLBACK
#include <functional>
#endif

namespace Logger
{
#if _WIN32
	void OpenLogFile(const std::wstring& name);
#else
	void OpenLogFile(const std::string& name);
#endif
	bool IsLogOpen();
	void SetTimestamp(bool);
	void SetConsoleOut(bool);
	void SetTitle(const std::string_view&);

	void Log(const std::string_view& str);
	void Log(const std::wstring_view& str);
	void LogWarn(const std::string_view& str);
	void LogWarn(const std::wstring_view& str);
	void LogError(const std::string_view& str);
	void LogError(const std::wstring_view& str);

	template <class... Args>
	inline void Log(const std::format_string<Args...> fmt, Args&&... _Args)
	{
		Log(std::vformat(fmt.get(), std::make_format_args(_Args...)));
	}

	template <class... Args>
	inline void Log(const std::wformat_string<Args...> fmt, Args&&... _Args)
	{
		Log(std::vformat(fmt.get(), std::make_wformat_args(_Args...)));
	}

	template <class... Args>
	inline void LogWarn(const std::format_string<Args...> fmt, Args&&... _Args)
	{
		LogWarn(std::vformat(fmt.get(), std::make_format_args(_Args...)));
	}

	template <class... Args>
	inline void LogWarn(const std::wformat_string<Args...> fmt, Args&&... _Args)
	{
		LogWarn(std::vformat(fmt.get(), std::make_wformat_args(_Args...)));
	}

	template <class... Args>
	inline void LogError(const std::format_string<Args...> fmt, Args&&... _Args)
	{
		LogError(std::vformat(fmt.get(), std::make_format_args(_Args...)));
	}

	template <class... Args>
	inline void LogError(const std::wformat_string<Args...> fmt, Args&&... _Args)
	{
		LogError(std::vformat(fmt.get(), std::make_wformat_args(_Args...)));
	}

#if _DEBUG
	void LogDebug(const std::string_view& str);
	void LogDebug(const std::wstring_view& str);

	template <class... Args>
	inline void LogDebug(const std::format_string<Args...> fmt, Args&&... _Args)
	{
		LogDebug(std::vformat(fmt.get(), std::make_format_args(_Args...)));
	}

	template <class... Args>
	inline void LogDebug(const std::wformat_string<Args...> fmt, Args&&... _Args)
	{
		LogDebug(std::vformat(fmt.get(), std::make_wformat_args(_Args...)));
	}
#else
	inline void LogDebug(const std::string_view& str)
	{
	}
	inline void LogDebug(const std::wstring_view& str)
	{
	}

	template <class... Args>
	inline void LogDebug(const std::format_string<Args...> fmt, Args&&... _Args)
	{
	}

	template <class... Args>
	inline void LogDebug(const std::wformat_string<Args...> fmt, Args&&... _Args)
	{
	}
#endif

	enum class LogLevel
	{
		Debug,
		Info,
		Warning,
		Error,
		Custom,
	};

	struct RGB
	{
		unsigned char r, g, b;
	};

	void LogEx(const std::string_view& str, RGB rgb, LogLevel level = LogLevel::Custom);
	void LogEx(const std::wstring_view& str, RGB rgb, LogLevel level = LogLevel::Custom);

#ifdef LOGGER_ENABLE_CALLBACK
	typedef std::function<void(const std::string_view&, LogLevel level)> Callback;
	Callback SetLogCallback(const Callback& callback);
#endif
};
