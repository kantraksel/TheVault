#pragma once
#include "IGame.h"

#ifdef GF_INCLUDE_WNDMGR
#include "Systems/WindowManager.h"
#endif

#ifdef GF_INCLUDE_GRAPHICS
#include "Systems/GraphicsEngine.h"
#endif

struct GhostFries
{
	GhostFries() = delete;
	static bool Main(IGame* pGame, const wchar_t* config = nullptr);
	static void Quit();
	static bool IsRunning();

	static class WindowManager& GetWindowManager();
	static class GraphicsEngine& GetRenderEngine();
};
