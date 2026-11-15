#pragma once
#include "Engine/IGame.h"
#include "GUI/GUIManager.h"
#include "GUI/MainWindow.h"
#include "GUI/VaultCommander.hpp"
#include "Vault/Vault.h"
#include "Vault/VaultKeeper.h"
#include "Vault/WorkerThread.h"
#include "ContentStore.h"
#include "UnsavedState.h"

class Game : public IGame
{
private:
	GUIManager gui;
	MainWindow mainWnd;
	VaultCommander vaultCommander;
	Vault vault;
	VaultKeeper keeper;
	WorkerThread worker;
	ContentStore contentStore;
	UnsavedState unsavedState;

	bool OnClose();

public:
	Game();
	~Game();

	bool OnInitialize() override;
	bool OnShutdown() override;

	auto& GetMainWindow() { return mainWnd; }
	auto& GetVaultCommander() { return vaultCommander; }
	auto& GetVault() { return vault; }
	auto& GetKeeper() { return keeper; }
	auto& GetWorker() { return worker; }
	auto& GetContentStore() { return contentStore; }
	auto& GetUnsavedState() { return unsavedState; }
};
