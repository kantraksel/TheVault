#include "BaseIndexModal.hpp"

BaseIndexModal::BaseIndexModal(const char* modalId) : BaseModal(modalId), selectedIndex{}
{
}

void BaseIndexModal::Open(int index)
{
	openNextFrame = true;
	selectedIndex = index;
}

void BaseIndexModal::Open()
{
}
