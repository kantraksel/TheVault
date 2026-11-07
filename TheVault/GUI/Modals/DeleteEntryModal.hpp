#pragma once
#include "BaseIndexModal.hpp"

class DeleteEntryModal : public BaseIndexModal
{
public:
	DeleteEntryModal();

protected:
	void OnRender() override;
};
