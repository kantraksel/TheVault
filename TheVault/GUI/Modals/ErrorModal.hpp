#pragma once
#include <string>
#include "BaseModal.hpp"

class ErrorModal : public BaseModal
{
public:
	ErrorModal();
	void Open(const std::string_view& message);

protected:
	std::string message;
	void Open() override;
	void OnRender() override;
};
