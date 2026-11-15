#pragma once
#include <string>
#include "BaseModal.hpp"

class ErrorModal : public BaseModal
{
public:
	ErrorModal();
	void Open(const std::string_view& message, bool critical = false);

protected:
	std::string message;
	bool isErrorCritical;
	void Open() override;
	void OnRender() override;
	void OnClose() override;
};
