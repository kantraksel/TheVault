#pragma once
#include <string>
#include "BaseIndexModal.hpp"

class ChangeLockPasswordModal : public BaseIndexModal
{
public:
	ChangeLockPasswordModal();

protected:
	void OnRender() override;
	void OnClose() override;

	std::string pwdInput;
	bool isProcessing;
};
