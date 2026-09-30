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
		mainWnd.OpenConfirmExitModal();
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
	
	//MORE LOGS!
	//handle all errornous cases properly (so app doesn't deadlock)
	//passmanager - incorrect error handling
	
	//fix saving oversized content buffer (cause unknown, happened once)
	//fix keyboard-only accessibility
	
	//zero passwords before destroying them
	//clipboard api
	//reset password in clipboard after 1 min
	//set file size limit to 100MB
	//pad store to 1KB
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

