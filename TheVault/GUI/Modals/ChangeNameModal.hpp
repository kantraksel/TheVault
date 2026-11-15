#pragma once
#include <string>
#include "BaseDualIndexModal.hpp"

class ChangeNameModal : public BaseDualIndexModal
{
public:
	ChangeNameModal();

protected:
	void OnRender() override;
	void OnOpen() override;
	void OnClose() override;

	std::string nameInput;
};
