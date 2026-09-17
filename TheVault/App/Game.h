#pragma once
#include "Engine/IGame.h"
#include "Utility/ObjectPointer.h"
#include "GUI/GUIManager.h"
#include "GUI/MainWindow.h"
#include "Vault/Vault.h"
#include "Vault/VaultKeeper.h"
#include "PassManager.h"
#include "UnsavedState.h"

class Game : public IGame
{
private:
	ObjectPointer<GUIManager> gui;
	MainWindow mainWnd;
	Vault vault;
	VaultKeeper keeper;
	PassManager passMgr;
	UnsavedState unsavedState;

	bool OnClose();

public:
	Game();
	~Game();

	void OnRegisterComponents(PrefabFactory& factory) override;
	bool OnInitialize() override;
	bool OnShutdown() override;

	auto& GetMainWindow() { return mainWnd; }
	auto& GetVault() { return vault; }
	auto& GetKeeper() { return keeper; }
	auto& GetPassManager() { return passMgr; }
	auto& GetUnsavedState() { return unsavedState; }
};
