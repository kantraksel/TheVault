#pragma once
#include "BaseIndexModal.hpp"

class ShowPasswordModal : public BaseIndexModal
{
public:
	ShowPasswordModal();

protected:
	void OnRender() override;
};
