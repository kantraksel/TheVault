#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>
#include "Systems/WindowManager.h"
#include "Engine/Logger.h"

#pragma comment(lib, "dwmapi")

constexpr std::wstring_view WindowClass = L"GhostFries Engine";

WindowManager::WindowManager() : mWnd(NULL), mIsMinimized(true), mTitle(WindowClass)
{
}

WindowManager::~WindowManager()
{
}

bool WindowManager::Initialize()
{
	auto hInstance = GetModuleHandle(NULL);

	if (!RegisterWndClass(hInstance))
	{
		Logger::LogError("Failed to register window class: {:X}", GetLastError());
		return false;
	}

	if (!InitWnd(hInstance))
	{
		Logger::LogError("Failed to create window: {:X}", GetLastError());
		return false;
	}

	Logger::Log("Created main window");
	return true;
}

void WindowManager::Shutdown()
{
	if (mWnd)
	{
		DestroyWindow(mWnd);
		mWnd = NULL;
	}

	Logger::Log("Destroyed main window");
}

void WindowManager::SetVisibility(bool value)
{
	if (!mWnd)
		return;
	ShowWindow(mWnd, SW_SHOW);
	UpdateWindow(mWnd);
}

void WindowManager::Resize(unsigned short width, unsigned short height)
{
	if (!mWnd)
		return;
	SetWindowPos(mWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
}

void WindowManager::Close()
{
	if (!mWnd)
		return;
	CloseWindow(mWnd);
}

void WindowManager::Update()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool WindowManager::RegisterWndClass(HINSTANCE hInst)
{
	WNDCLASSEX wcex
	{
		.cbSize = sizeof(WNDCLASSEX),
		.style = 0,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = sizeof(uintptr_t),
		.hInstance = hInst,
		.hIcon = LoadIcon(nullptr, IDI_APPLICATION),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)),
		.lpszMenuName = nullptr,
		.lpszClassName = WindowClass.data(),
		.hIconSm = LoadIcon(nullptr, IDI_APPLICATION),
	};

	return RegisterClassEx(&wcex) != 0;
}

bool WindowManager::InitWnd(HINSTANCE hInst)
{
	constexpr DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	mWnd = CreateWindowEx(WS_EX_CONTROLPARENT, WindowClass.data(), mTitle.data(), style, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);

	if (!mWnd)
		return false;

	SetWindowLongPtr(mWnd, 0, reinterpret_cast<LONG_PTR>(this));

	BOOL value = TRUE;
	DwmSetWindowAttribute(mWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));

	return true;
}

LRESULT CALLBACK WindowManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto* pInstance = reinterpret_cast<WindowManager*>(GetWindowLongPtr(hWnd, 0));
	if (!pInstance)
		return DefWindowProc(hWnd, message, wParam, lParam);

	switch (message)
	{
		case WM_CLOSE:
		{
			auto& callback = pInstance->mOnClose;
			if (!callback || callback())
				DestroyWindow(hWnd);
			break;
		}

		case WM_DESTROY:
		{
			pInstance->mWnd = NULL;
			auto& callback = pInstance->mOnDestroy;
			if (callback)
				callback();
			break;
		}

		case WM_SIZE:
		{
			switch (wParam)
			{
				case SIZE_MINIMIZED:
					pInstance->mIsMinimized = true;
					break;

				case SIZE_RESTORED:
				case SIZE_MAXIMIZED:
					pInstance->mIsMinimized = false;
					break;
			}
			break;
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void WindowManager::SetWindowTitle(const wchar_t* title)
{
	mTitle = title;
	if (!mWnd)
		return;
	SetWindowText(mWnd, title);
}

static void ShowMessageBox(const wchar_t* message, UINT type)
{
	std::wstring name;
	HWND wnd = GetForegroundWindow();

	if (wnd)
	{
		auto size = GetWindowTextLength(wnd);
		if (size > 0)
		{
			name.resize(size + 1);
			size = GetWindowText(wnd, name.data(), (int)name.size());
			name.resize(size);
		}
	}

	if (name.empty())
		name = WindowClass;

	MessageBox(wnd, message, name.c_str(), type | MB_OK);
}

void WindowManager::ShowError(const wchar_t* message)
{
	Logger::LogError(L"WindowManager::ShowError: {}", message);
	ShowMessageBox(message, MB_ICONERROR);
}

void WindowManager::ShowInfo(const wchar_t* message)
{
	Logger::Log(L"WindowManager::ShowInfo: {}", message);
	ShowMessageBox(message, MB_ICONINFORMATION);
}

void WindowManager::SetCloseCallback(const Function<bool()>& callback)
{
	mOnClose = callback;
}

void WindowManager::SetDestroyCallback(const Function<void()>& callback)
{
	mOnDestroy = callback;
}
