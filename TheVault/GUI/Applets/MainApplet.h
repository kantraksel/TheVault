#pragma once
#include "IApplet.h"

class MainApplet : public IApplet
{
private:
	void RenderMenuBar();
	void ProcessShortcuts();

public:
	void Render() override;
};
