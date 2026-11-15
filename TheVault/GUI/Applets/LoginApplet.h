#pragma once
#include <string>

class LoginApplet
{
protected:
	std::string passwordInput;
	std::string hint;
	bool isOpen;
	bool firstFrameDraw;
	bool isProcessing;

public:
	LoginApplet();
	~LoginApplet();

	void Open();
	void Close();
	void Render();
	void RefreshHintName();

	bool IsOpen()
	{
		return isOpen;
	}
};
