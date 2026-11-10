#pragma once
#include "BaseDualIndexModal.hpp"

class DeleteEntryModal : public BaseDualIndexModal
{
public:
	DeleteEntryModal();

protected:
	void OnRender() override;
};
