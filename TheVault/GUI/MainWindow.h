#pragma once
#include "IRender.h"
#include "Applets/MainApplet.h"
#include "Modals.hpp"

class MainWindow : public IRender
{
private:
	MainApplet main;
	Modals modals;

	void RenderAppletBar();

public:
	MainWindow();
	~MainWindow();

	void Render() override;

	auto& GetMainApplet()
	{
		return main;
	}

	auto& GetModals()
	{
		return modals;
	}
};
