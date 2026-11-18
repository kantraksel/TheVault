#pragma once
#include "BaseModal.hpp"

class ConfirmExitModal : public BaseModal
{
public:
	static ConfirmExitModal& Get();
	ConfirmExitModal();

	void Open() override;
	void Open(bool closeAppOnConfirm);

protected:
	void OnRender() override;

	bool closeAppOnConfirmExit;
};
