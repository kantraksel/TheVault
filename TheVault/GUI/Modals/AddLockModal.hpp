#pragma once
#include <string>
#include "BaseModal.hpp"

class AddLockModal : public BaseModal
{
public:
	AddLockModal();

protected:
	void OnRender() override;
	void OnClose() override;

	std::string nameInput;
	std::string pwdInput;
	bool isProcessing;
};
