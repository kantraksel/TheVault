#pragma once
#include <string>
#include <future>
#include "BaseModal.hpp"

class AddLockModal : public BaseModal
{
public:
	AddLockModal();

protected:
	void OnRender() override;

	std::string nameInput;
	std::string pwdInput;
	std::future<uint64_t> vaultTask;
};
