#include <d3d11.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include "GUIManager.h"
#include "Engine/Logger.h"
#define GF_INCLUDE_WNDMGR
#define GF_INCLUDE_RESOURCES
#define GF_INCLUDE_GRAPHICS
#include "Engine/GhostFries.h"

// forward declare from imgui_impl_win32.h
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT WndHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return 1;

	return WindowManager::WndProc(hWnd, message, wParam, lParam);
}

GUIManager::GUIManager(Actor* pParent) : ICanvasComponent(pParent)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = nullptr;
	ImGui::StyleColorsDark();
}

GUIManager::~GUIManager()
{
	Shutdown();
	ImGui::DestroyContext();
}

bool GUIManager::Initialize()
{
	if (!InitImguiBackends() || !SetupWindow())
	{
		Logger::LogError("Failed to init ImGui backend");
		return false;
	}
	return true;
}

bool GUIManager::InitImguiBackends()
{
	auto& io = ImGui::GetIO();

	if (!io.BackendPlatformUserData)
	{
		auto& wnd = GhostFries::GetWindowManager();
		if (!ImGui_ImplWin32_Init(wnd.GetWindow()))
			return false;
	}
	
	if (!io.BackendRendererUserData)
	{
		auto& ctx = GhostFries::GetRenderEngine().GetRenderContext();
		auto* pContext = ctx.GetDeviceContext();
		ID3D11Device* pDevice;
		pContext->GetDevice(&pDevice);

		bool result = ImGui_ImplDX11_Init(pDevice, pContext);
		pDevice->Release();
		if (!result)
			return false;
	}
	return true;
}

bool GUIManager::SetupWindow()
{
	auto& ctx = GhostFries::GetRenderEngine().GetRenderContext();
	auto& wnd = GhostFries::GetWindowManager();

	SetLastError(0);
	auto fnOrig = SetWindowLongPtr(wnd.GetWindow(), GWLP_WNDPROC, (LONG_PTR)&WndHandler);
	auto val = fnOrig == 0 ? GetLastError() : 0;
	if (val != 0)
	{
		Logger::LogError("Failed to set custom WndProc");
		return false;
	}

	int sizeX = 800;
	int sizeY = 600;

	auto screenX = GetSystemMetrics(SM_CXFULLSCREEN);
	auto screenY = GetSystemMetrics(SM_CYFULLSCREEN);
	if (!SetWindowPos(wnd.GetWindow(), HWND_TOP, (screenX - sizeX) / 2, (screenY - sizeY) / 2, 0, 0, SWP_NOSIZE))
	{
		Logger::Log("Failed to center window");
		return false;
	}
	return ctx.SetDisplayDimensions(sizeX, sizeY);
}

void GUIManager::Shutdown()
{
	objects.clear();

	auto& io = ImGui::GetIO();
	if (io.BackendRendererUserData)
		ImGui_ImplDX11_Shutdown();
	if (io.BackendPlatformUserData)
		ImGui_ImplWin32_Shutdown();
}

void GUIManager::Render(D2DEngine& engine)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (auto* obj : objects)
	{
		obj->Render();
	}
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GUIManager::RegisterObject(IRender* obj)
{
	objects.push_back(obj);
}
