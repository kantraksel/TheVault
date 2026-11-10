#pragma once
#include "IApplet.h"
#include "LockApplet.h"

class MainApplet : public IApplet
{
private:
	void RenderMenuBar();
	void ProcessShortcuts();
	LockApplet lockApplet;

public:
	void Render() override;

	auto& GetLockApplet()
	{
		return lockApplet;
	}
};
