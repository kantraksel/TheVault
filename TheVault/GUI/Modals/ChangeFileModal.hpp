#pragma once
#include <string>
#include "BaseIndexModal.hpp"

class ChangeFileModal : public BaseIndexModal
{
public:
	ChangeFileModal(class ChangeNameModal& changeNameModal);

protected:
	void OnRender() override;
	void OnClose() override;

	std::wstring filePath;
	std::string fileName;
	ChangeNameModal& changeNameModal;
};
