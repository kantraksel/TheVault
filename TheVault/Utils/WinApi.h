#pragma once
#include <string>

namespace WinApi
{
	bool OpenFileDialog(const wchar_t* title, const std::wstring_view& defaultName, std::wstring& path);
	bool SaveFileDialog(const wchar_t* title, const std::wstring_view& defaultName, std::wstring& path);
}
