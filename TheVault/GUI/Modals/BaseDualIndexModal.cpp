#include "BaseDualIndexModal.hpp"

BaseDualIndexModal::BaseDualIndexModal(const char* modalId) : BaseIndexModal(modalId), isLockIndex{}
{
}

void BaseDualIndexModal::Open(int index)
{
	BaseIndexModal::Open(index);
	isLockIndex = false;
}

void BaseDualIndexModal::Open(int index, bool isLock)
{
	BaseIndexModal::Open(index);
	isLockIndex = isLock;
}
