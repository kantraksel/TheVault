#pragma once
#include <string>
#include "BlockingApplet.hpp"

class LoginApplet : public BlockingApplet
{
protected:
	std::string passwordInput;
	std::string hint;
	bool firstFrameDraw;
	bool isProcessing;

	void OnRender() override;

public:
	static LoginApplet& Get();

	LoginApplet(MainWindow& mainWindow);
	~LoginApplet();

	void Open() override;
	void Close() override;
	void RefreshHintName();

	bool IsOpen()
	{
		return isOpen;
	}
};
