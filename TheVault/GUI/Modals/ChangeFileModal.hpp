#pragma once
#include <string>
#include "BaseIndexModal.hpp"

class ChangeFileModal : public BaseIndexModal
{
public:
	ChangeFileModal();

protected:
	void OnRender() override;

	std::wstring filePath;
	std::string fileName;
};
