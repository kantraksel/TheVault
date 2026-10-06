#define GF_INCLUDE_GRAPHICS
#define GF_INCLUDE_WNDMGR
#include "Engine/GhostFries.h"
#include "Engine/Logger.h"
#undef ZeroMemory
#undef CopyMemory
#include "Vault/Crypto.h"
#include "Game.h"

Game::Game() : contentStore(unsavedState)
{
}

Game::~Game()
{
}

bool Game::OnClose()
{
	if (unsavedState.HasChanged())
	{
		mainWnd.OpenConfirmExitModal(true);
		return false;
	}
	return true;
}

bool Game::OnInitialize()
{
	GhostFries::GetWindowManager().SetCloseCallback({ MemberFunc<&Game::OnClose>, this });

	if (!gui.Initialize())
		return false;

	gui.RegisterObject(&mainWnd);
	mainWnd.Initialize();
	worker.Init();

	//UX redesign (+keyboard-only accessibility, welcome screen tooltip)
	//move to Vulkan
	//*support Linux
	//reset password in clipboard after 1 min
	return true;
}

Game game;

bool Game::OnShutdown()
{
	worker.Shutdown();
	gui.Shutdown();
	return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (!Crypto::Init())
		return -1;

	return GhostFries::Main(&game, L"name: 'TheVault'\n");
}

