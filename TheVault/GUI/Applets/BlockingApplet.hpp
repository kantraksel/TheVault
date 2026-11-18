#pragma once

class BlockingApplet
{
protected:
	class MainWindow& parent;
	bool isOpen;

	virtual void OnRender() = 0;

public:
	BlockingApplet(MainWindow& mainWindow);
	~BlockingApplet();

	virtual void Open();
	virtual void Close();
	void Render();
};
