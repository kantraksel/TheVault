#pragma once
#include <string>
#include <future>
#include "BaseIndexModal.hpp"

class ChangeLockPasswordModal : public BaseIndexModal
{
public:
	ChangeLockPasswordModal();

protected:
	void OnRender() override;

	std::string pwdInput;
	std::future<uint64_t> vaultTask;
};
