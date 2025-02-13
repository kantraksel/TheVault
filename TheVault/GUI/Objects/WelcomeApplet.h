#pragma once
#include "IApplet.h"

class WelcomeApplet : public IApplet
{
private:
public:
	WelcomeApplet();
	~WelcomeApplet();

	void Render() override;
};
