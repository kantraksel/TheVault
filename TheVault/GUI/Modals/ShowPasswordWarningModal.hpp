#pragma once
#include "BaseIndexModal.hpp"

class ShowPasswordWarningModal : public BaseIndexModal
{
public:
	ShowPasswordWarningModal();

protected:
	void OnRender() override;
};
