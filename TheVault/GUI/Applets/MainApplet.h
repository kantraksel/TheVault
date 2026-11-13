#pragma once
#include "IApplet.h"
#include "LockApplet.h"
#include "LoginApplet.h"

class MainApplet : public IApplet
{
private:
	void RenderMenuBar();
	void ProcessShortcuts();
	LockApplet lockApplet;
	LoginApplet loginApplet;
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
};
