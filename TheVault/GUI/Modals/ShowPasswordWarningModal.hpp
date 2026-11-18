#pragma once
#include "BaseIndexModal.hpp"

class ShowPasswordWarningModal : public BaseIndexModal
{
public:
	ShowPasswordWarningModal(class ShowPasswordModal& showPasswordModal);

protected:
	void OnRender() override;

	ShowPasswordModal& showPasswordModal;
};
