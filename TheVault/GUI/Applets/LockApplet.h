#pragma once

class LockApplet
{
private:
	bool isOpen;

public:
	LockApplet();
	~LockApplet();

	void Open();
	void Close();
	void Render();

	bool IsOpen()
	{
		return isOpen;
	}
};
