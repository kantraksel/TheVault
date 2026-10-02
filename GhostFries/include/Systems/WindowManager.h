#pragma once
#include <string>
#include "Utility/Base.h"
#include "Utility/Function.h"

typedef struct HWND__* HWND;
typedef struct HINSTANCE__* HINSTANCE;
typedef unsigned long long WPARAM;
typedef long long LPARAM;
typedef long long LRESULT;
typedef unsigned int UINT;

class WindowManager final : LockedObject
{
private:
	HWND mWnd;
	std::wstring mTitle;
	bool mIsMinimized;
	Function<bool()> mOnClose;
	Function<void()> mOnDestroy;

	bool RegisterWndClass(HINSTANCE hInst);
	bool InitWnd(HINSTANCE hInst);

public:
	// allow to customize window behaviour
	static LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	WindowManager();
	~WindowManager();

	bool Initialize();
	void Update();
	void Shutdown();

	void SetVisibility(bool value);
	void Resize(unsigned short width, unsigned short height);
	void Close();

	void SetWindowTitle(const wchar_t* title);
	HWND GetWindow() { return mWnd; }
	bool IsMinimized() { return mIsMinimized; }
	void SetCloseCallback(const Function<bool()>& callback);
	void SetDestroyCallback(const Function<void()>& callback);

	static void ShowError(const wchar_t* message);
	static void ShowInfo(const wchar_t* message);
};
