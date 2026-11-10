#pragma once
#include "BaseModal.hpp"

class BaseIndexModal : public BaseModal
{
public:
	BaseIndexModal(const char* modalId);
	virtual void Open(int index);

protected:
	int selectedIndex;
	void Open() override;
};

