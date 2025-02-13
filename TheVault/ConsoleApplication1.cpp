#define GF_INCLUDE_SCENEMGR
#define GF_INCLUDE_GRAPHICS
#define GF_INCLUDE_WNDMGR
#include "Engine/GhostFries.h"
#include "Engine/Logger.h"
#undef ZeroMemory
#undef CopyMemory
#include "Crypto.h"
#include "Game.h"
#include "Engine/Components/CameraComponent.h"

Game::Game() : passMgr(unsavedState)
{
}

Game::~Game()
{
}

void Game::OnRegisterComponents(PrefabFactory& factory)
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
	GhostFries::GetWindowManager().SetTryCloseEvent(Function<bool, true>{ entt::delegate(entt::connect_arg<&Game::OnClose>, this) });

	auto actor = GhostFries::GetSceneManager().GetActiveScene()->AddActor();
	actor->AddComponent<CameraComponent>();
	gui = actor->AddComponent<GUIManager>();

	if (!gui->Initialize() || !vault.Initialize())
		return false;

	gui->RegisterObject(&mainWnd);
	mainWnd.Initialize();
	keeper.Init();
	
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
	keeper.Shutdown();
	gui->Shutdown();
	return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (!Crypto::Init())
		return -1;

	return GhostFries::Main(hInstance, &game);
}

