#pragma once
#include <string>
#include "BaseIndexModal.hpp"

class ChangeNameModal : public BaseIndexModal
{
public:
	ChangeNameModal();

protected:
	void OnRender() override;
	void OnOpen() override;

	std::string nameInput;
};
