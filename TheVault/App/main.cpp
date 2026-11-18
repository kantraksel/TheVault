#define GF_INCLUDE_GRAPHICS
#define GF_INCLUDE_WNDMGR
#include "Engine/GhostFries.h"
#include "Engine/Logger.h"
#undef ZeroMemory
#include "Vault/Crypto.h"
#include "Engine/IGame.h"
#include "GUI/GUIManager.h"
#include "GUI/MainWindow.h"
#include "GUI/VaultCommander.hpp"
#include "Vault/VaultKeeper.h"
#include "ContentStore.h"
#include "UnsavedState.h"

class Game : public IGame
{
public:
	GUIManager gui;
	MainWindow mainWnd;
	VaultCommander commander;
	VaultKeeper keeper;
	ContentStore contentStore;
	UnsavedState unsavedState;

	Game();
	bool OnInitialize() override;
	bool OnShutdown() override;
};

Game::Game() : commander(unsavedState), keeper(contentStore, unsavedState), contentStore(unsavedState)
{
}

bool Game::OnInitialize()
{
	GhostFries::GetWindowManager().SetCloseCallback([this]
		{
			commander.TryCloseVault(true);
			return false;
		});

	if (!gui.Initialize({ MemberFunc<&MainWindow::Render>, &mainWnd }))
		return false;

	keeper.Initialize();
	return true;
}

bool Game::OnShutdown()
{
	keeper.Shutdown();
	gui.Shutdown();
	return true;
}

static Game game;

MainWindow& MainWindow::Get()
{
	return game.mainWnd;
}

VaultCommander& VaultCommander::Get()
{
	return game.commander;
}

VaultKeeper& VaultKeeper::Get()
{
	return game.keeper;
}

ContentStore& ContentStore::Get()
{
	return game.contentStore;
}

ErrorModal& ErrorModal::Get()
{
	return game.mainWnd.GetErrorModal();
}

ConfirmExitModal& ConfirmExitModal::Get()
{
	return game.mainWnd.GetConfirmExitModal();
}

LockApplet& LockApplet::Get()
{
	return game.mainWnd.GetLockApplet();
}

LoginApplet& LoginApplet::Get()
{
	return game.mainWnd.GetLoginApplet();
}

ProcessApplet& ProcessApplet::Get()
{
	return game.mainWnd.GetProcessApplet();
}

WelcomeApplet& WelcomeApplet::Get()
{
	return game.mainWnd.GetWelcomeApplet();
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

