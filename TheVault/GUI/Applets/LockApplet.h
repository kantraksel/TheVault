#pragma once
#include <future>
#include "Utils/SecureArray.h"
#include "IApplet.h"

class LockApplet : public IApplet
{
private:
	bool openSetHintValueModal : 1;
	bool openChangeHintModal : 1;
	bool openDeleteModal : 1;
	int modalIdx;

	std::string nameInput;
	SecureArray passwordInput;
	std::future<uint64_t> vaultTask;

	void OpenSetHintValueModal(int idx);
	void OpenChangeHintModal(int idx);
	void OpenDeleteModal(int idx);
	
	void RenderMain();
	void RenderSetHintValueModal();
	void RenderChangeHintModal();
	void RenderDeleteModal();

public:
	LockApplet();
	~LockApplet();

	void Initialize();
	void OnLeave() override;
	void Render() override;
};
