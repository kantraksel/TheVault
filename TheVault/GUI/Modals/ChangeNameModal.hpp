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

	std::string nameInput;
};
