#pragma once
#include <string>

namespace StringUtils
{
    std::wstring Utf8ToWideString(const std::string_view& str);
    std::string WideStringToUtf8(const std::wstring_view& str);

    std::string ToString(unsigned long long n);
    std::string ToString(long long n);

    inline std::string ToString(unsigned int n)
    {
        return ToString((unsigned long long)n);
    }
    inline std::string ToString(int n)
    {
        return ToString((long long)n);
    }

	std::string Trim(const std::string& str);

#ifdef _HRESULT_DEFINED
#if _DEBUG
	std::string Format(HRESULT hr);
	std::wstring FormatW(HRESULT hr);
#else
	inline std::string Format(HRESULT hr)
	{
		return std::format("{:X}", hr);
	}
	inline std::wstring FormatW(HRESULT hr)
	{
		return std::format(L"{:X}", hr);
	}
#endif
#endif
}
