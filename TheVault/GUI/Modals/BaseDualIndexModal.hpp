#pragma once
#include "BaseIndexModal.hpp"

class BaseDualIndexModal : public BaseIndexModal
{
public:
	BaseDualIndexModal(const char* modalId);
	void Open(int index) override;
	void Open(int index, bool isLock);

protected:
	bool isLockIndex;
};
