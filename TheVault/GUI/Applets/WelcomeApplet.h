#pragma once
#include "BlockingApplet.hpp"

class WelcomeApplet : public BlockingApplet
{
protected:
	void OnRender() override;

public:
	static WelcomeApplet& Get();

	WelcomeApplet(MainWindow& mainWindow);
	~WelcomeApplet();
};
