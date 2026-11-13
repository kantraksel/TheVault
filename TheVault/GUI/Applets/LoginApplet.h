#pragma once
#include <string>
#include <future>

class LoginApplet
{
protected:
	std::string passwordInput;
	std::string hint;
	std::future<uint64_t> vaultTask;
	bool isOpen;
	bool firstFrameDraw;

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
