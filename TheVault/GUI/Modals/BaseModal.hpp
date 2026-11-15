#pragma once

class BaseModal
{
public:
	BaseModal(const char* modalId);

	virtual void Open();
	void Close();
	void Render();

protected:
	const char* modalId;
	bool openNextFrame;
	bool firstFrameDraw;
	bool closeNextFrame;

	virtual void OnRender() = 0;
	virtual void OnOpen();
	virtual void OnClose();
};
