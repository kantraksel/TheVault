#pragma once
#include "BaseIndexModal.hpp"

class ShowPasswordModal : public BaseIndexModal
{
public:
	ShowPasswordModal(class ChangePasswordModal& changePasswordModal);

protected:
	void OnRender() override;

	ChangePasswordModal& changePasswordModal;
};
