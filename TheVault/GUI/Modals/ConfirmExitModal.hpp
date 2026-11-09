#pragma once
#include "BaseModal.hpp"

class ConfirmExitModal : public BaseModal
{
public:
	ConfirmExitModal();

	void Open() override;
	void Open(bool closeAppOnConfirm);

protected:
	void OnRender() override;

	bool closeAppOnConfirmExit;
};
