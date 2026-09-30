#pragma once
#include "Engine/IGame.h"
#include "GUI/GUIManager.h"
#include "GUI/MainWindow.h"
#include "Vault/Vault.h"
#include "Vault/VaultKeeper.h"
#include "Vault/WorkerThread.h"
#include "PassManager.h"
#include "UnsavedState.h"

class Game : public IGame
{
private:
	GUIManager gui;
	MainWindow mainWnd;
	Vault vault;
	VaultKeeper keeper;
	WorkerThread worker;
	PassManager passMgr;
	UnsavedState unsavedState;

	bool OnClose();

public:
	Game();
	~Game();

	bool OnInitialize() override;
	bool OnShutdown() override;

	auto& GetMainWindow() { return mainWnd; }
	auto& GetVault() { return vault; }
	auto& GetKeeper() { return keeper; }
	auto& GetWorker() { return worker; }
	auto& GetPassManager() { return passMgr; }
	auto& GetUnsavedState() { return unsavedState; }
};
