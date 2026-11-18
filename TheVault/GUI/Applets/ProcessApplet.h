#pragma once
#include <string>
#include "BlockingApplet.hpp"

class ProcessApplet : public BlockingApplet
{
protected:
	std::string title;

	void OnRender() override;

public:
	static ProcessApplet& Get();

	ProcessApplet(MainWindow& mainWindow);
	~ProcessApplet();

	void Open(const std::string_view& title);
	void Close() override;
};
