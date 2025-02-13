#include <Windows.h>
#define GF_INCLUDE_WNDMGR
#include "Engine/GhostFries.h"
#include "WinApi.h"

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
