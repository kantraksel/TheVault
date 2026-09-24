#include <Windows.h>
#define GF_INCLUDE_WNDMGR
#include "Engine/GhostFries.h"
#include "WinApi.h"
#include "Utility/StringUtils.h"

bool WinApi::OpenFileDialog(const wchar_t* title, const std::wstring_view& defaultName, std::wstring& path)
{
	auto name = std::wstring(defaultName);
	name.resize(65536);

	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GhostFries::GetWindowManager().GetWindow();
	ofn.lpstrFile = name.data();
	ofn.nMaxFile = (DWORD)name.size();
	ofn.lpstrFilter = L"All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = nullptr;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT;
	if (GetOpenFileName(&ofn))
	{
		path = name.data();
		return true;
	}
	path.clear();
	return false;
}

bool WinApi::SaveFileDialog(const wchar_t* title, const std::wstring_view& defaultName, std::wstring& path)
{
	auto name = std::wstring(defaultName);
	name.resize(65536);

	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GhostFries::GetWindowManager().GetWindow();
	ofn.lpstrFile = name.data();
	ofn.nMaxFile = (DWORD)name.size();
	ofn.lpstrFilter = L"All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = nullptr;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_DONTADDTORECENT;
	if (GetSaveFileName(&ofn))
	{
		path = name.data();
		return true;
	}
	path.clear();
	return false;
}

bool WinApi::SetClipboardText(const std::string_view& text)
{
	if (!OpenClipboard(NULL))
		return false;

	auto data = StringUtils::Utf8ToWideString(text);

	auto dataSize = (data.size() + 1) * sizeof(wchar_t);
	auto hData = GlobalAlloc(GMEM_MOVEABLE, dataSize);
	if (!hData)
	{
		CloseClipboard();
		return false;
	}
	auto pData = GlobalLock(hData);
	memcpy(pData, data.c_str(), dataSize);
	GlobalUnlock(hData);

	EmptyClipboard();
	auto result = SetClipboardData(CF_UNICODETEXT, hData);
	CloseClipboard();

	if (result == NULL)
	{
		GlobalFree(hData);
		return false;
	}
	return true;
}
