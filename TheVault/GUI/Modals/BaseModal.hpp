#pragma once

class BaseModal
{
public:
	BaseModal(const char* modalId);

	virtual void Open();
	void Render();

protected:
	const char* modalId;
	bool openNextFrame;
	bool firstFrameDraw;

	virtual void OnRender() = 0;
	virtual void OnOpen();
};
