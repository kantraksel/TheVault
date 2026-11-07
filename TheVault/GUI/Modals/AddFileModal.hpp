#pragma once
#include <string>
#include "BaseModal.hpp"

class AddFileModal : public BaseModal
{
public:
	AddFileModal();

protected:
	void OnRender() override;

	std::string nameInput;
	std::wstring filePath;
	std::string fileName;
};
