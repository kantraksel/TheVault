#pragma once
#include "GUI/Modals/AddLockModal.hpp"
#include "GUI/Modals/ChangeLockPasswordModal.hpp"

class LockApplet
{
private:
	bool isOpen;
	AddLockModal addLockModal;
	ChangeLockPasswordModal changeLockPasswordModal;
	class DeleteEntryModal& deleteEntryModal;

public:
	static LockApplet& Get();

	LockApplet(DeleteEntryModal& deleteEntryModal, ChangeNameModal& changeNameModal);
	~LockApplet();

	void Open();
	void Close();
	void Render();

	bool IsOpen()
	{
		return isOpen;
	}
};
