#pragma once
#include <string>

class ProcessApplet
{
private:
	std::string title;
	bool isOpen;

public:
	ProcessApplet();
	~ProcessApplet();

	void Open(const std::string_view& title);
	void Close();
	void Render();
};
