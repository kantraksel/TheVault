#pragma once
#include "BlockingApplet.hpp"

class AboutApplet : public BlockingApplet
{
protected:
	void OnRender() override;

public:
	AboutApplet(MainWindow& mainWindow);
	~AboutApplet();
};
