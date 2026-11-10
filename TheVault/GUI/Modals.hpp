#pragma once
#include "Modals/AddPasswordModal.hpp"
#include "Modals/AddFileModal.hpp"
#include "Modals/ShowPasswordWarningModal.hpp"
#include "Modals/ShowPasswordModal.hpp"
#include "Modals/ChangePasswordModal.hpp"
#include "Modals/ChangeFileModal.hpp"
#include "Modals/ChangeNameModal.hpp"
#include "Modals/DeleteEntryModal.hpp"
#include "Modals/AddLockModal.hpp"
#include "Modals/ChangeLockPasswordModal.hpp"
#include "Modals/ErrorModal.hpp"
#include "Modals/ConfirmExitModal.hpp"

struct Modals
{
private:
	AddPasswordModal addPasswordModal;
	AddFileModal addFileModal;
	ShowPasswordWarningModal showPasswordWarningModal;
	ShowPasswordModal showPasswordModal;
	ChangePasswordModal changePasswordModal;
	ChangeFileModal changeFileModal;
	ChangeNameModal changeNameModal;
	DeleteEntryModal deleteEntryModal;
	AddLockModal addLockModal;
	ChangeLockPasswordModal changeLockPasswordModal;
	ConfirmExitModal confirmExitModal;
	ErrorModal errorModal;

public:
	void Render();

	static AddPasswordModal& GetAddPasswordModal();
	static AddFileModal& GetAddFileModal();
	static ShowPasswordWarningModal& GetShowPasswordWarningModal();
	static ShowPasswordModal& GetShowPasswordModal();
	static ChangePasswordModal& GetChangePasswordModal();
	static ChangeFileModal& GetChangeFileModal();
	static ChangeNameModal& GetChangeNameModal();
	static DeleteEntryModal& GetDeleteEntryModal();
	static AddLockModal& GetAddLockModal();
	static ChangeLockPasswordModal& GetChangeLockPasswordModal();
	static ConfirmExitModal& GetConfirmExitModal();
	static ErrorModal& GetErrorModal();
};
