#pragma once
#include <string>
#include "BaseIndexModal.hpp"

class ChangePasswordModal : public BaseIndexModal
{
public:
	ChangePasswordModal();

protected:
	void OnRender() override;

	std::string pwdInput;
};
