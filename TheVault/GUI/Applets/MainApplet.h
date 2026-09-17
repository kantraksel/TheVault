#pragma once
#include "IApplet.h"

class MainApplet : public IApplet
{
private:
	bool openSelectAddModal : 1;
	bool openAddTextModal : 1;
	bool openAddFileModal : 1;
	bool openShowTextBlockedModal : 1;
	bool openShowTextModal : 1;
	bool openChangeTextModal : 1;
	bool openChangeFileModal : 1;
	bool openChangeNameModal : 1;
	bool openDeleteModal : 1;
	int modalIdx;

	std::string nameInput;
	std::string pwdInput;
	std::wstring wBuffer;

	void OpenSelectAddModal();
	void OpenAddTextModal();
	void OpenAddFileModal();
	void OpenShowTextModal(int idx);
	void OpenShowTextUnblockedModal(int idx);
	void OpenChangeTextModal(int idx);
	void OpenChangeFileModal(int idx);
	void OpenChangeNameModal(int idx);
	void OpenDeleteModal(int idx);

	void RenderMain();
	void RenderSelectAddModal();
	void RenderAddTextModal();
	void RenderAddFileModal();
	void RenderShowTextBlockModal();
	void RenderShowTextModal();
	void RenderChangeTextModal();
	void RenderChangeFileModal();
	void RenderChangeNameModal();
	void RenderDeleteModal();

public:
	MainApplet();
	~MainApplet();

	void Render() override;
};
