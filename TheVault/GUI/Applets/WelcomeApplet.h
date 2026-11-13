#pragma once
#include <string>

class WelcomeApplet
{
private:
	std::wstring name;
	bool isOpen;

public:
	WelcomeApplet();
	~WelcomeApplet();

	void Open();
	void Close();
	void Render();
};
