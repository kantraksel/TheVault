#pragma once
#include "BlockingApplet.hpp"

class WelcomeApplet : public BlockingApplet
{
protected:
	void OnRender() override;
	bool openNextFrame;

public:
	static WelcomeApplet& Get();

	WelcomeApplet(MainWindow& mainWindow);
	~WelcomeApplet();

	void Open() override;
	void Close() override;
};
