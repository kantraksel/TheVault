#pragma once
#include <string>
#include "BlockingApplet.hpp"

class WelcomeApplet : public BlockingApplet
{
protected:
	std::wstring name;

	void OnRender() override;

public:
	static WelcomeApplet& Get();

	WelcomeApplet(MainWindow& mainWindow);
	~WelcomeApplet();

	void Close() override;
};
