#pragma once
#include <string>
#include "BaseModal.hpp"

class AddPasswordModal : public BaseModal
{
public:
	AddPasswordModal();

protected:
	void OnRender() override;

	std::string nameInput;
	std::string pwdInput;
};
