#pragma once
#include "Applets/LockApplet.h"
#include "Applets/LoginApplet.h"
#include "Applets/WelcomeApplet.h"
#include "Applets/ProcessApplet.h"
#include "Modals.hpp"

class MainWindow
{
private:
	LockApplet lockApplet;
	LoginApplet loginApplet;
	WelcomeApplet welcomeApplet;
	ProcessApplet processApplet;
	Modals modals;
	bool isDisabled;

	void RenderMainArea();
	void RenderMenuBar();
	void ProcessShortcuts();
	void RenderDemoBar();

public:
	MainWindow();
	~MainWindow();

	void Render();
	void DisableInteractions();
	void EnableInteractions();

	auto& GetLockApplet()
	{
		return lockApplet;
	}

	auto& GetLoginApplet()
	{
		return loginApplet;
	}

	auto& GetWelcomeApplet()
	{
		return welcomeApplet;
	}

	auto& GetProcessApplet()
	{
		return processApplet;
	}

	auto& GetModals()
	{
		return modals;
	}
};
