#pragma once
#include "IApplet.h"
#include "LockApplet.h"
#include "LoginApplet.h"
#include "WelcomeApplet.h"
#include "ProcessApplet.h"

class MainApplet : public IApplet
{
private:
	void RenderMenuBar();
	void ProcessShortcuts();
	LockApplet lockApplet;
	LoginApplet loginApplet;
	WelcomeApplet welcomeApplet;
	ProcessApplet processApplet;
	bool isDisabled = false;

public:
	void Render() override;

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
};
