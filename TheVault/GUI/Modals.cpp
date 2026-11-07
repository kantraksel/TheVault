#include "Modals.hpp"
#include "../App/Game.h"

extern Game game;

void Modals::Render()
{
	addPasswordModal.Render();
	addFileModal.Render();
	showPasswordWarningModal.Render();
	showPasswordModal.Render();
	changePasswordModal.Render();
	changeFileModal.Render();
	changeNameModal.Render();
	deleteEntryModal.Render();
	errorModal.Render();
}

AddPasswordModal& Modals::GetAddPasswordModal()
{
	return game.GetMainWindow().GetModals().addPasswordModal;
}

AddFileModal& Modals::GetAddFileModal()
{
	return game.GetMainWindow().GetModals().addFileModal;
}

ShowPasswordWarningModal& Modals::GetShowPasswordWarningModal()
{
	return game.GetMainWindow().GetModals().showPasswordWarningModal;
}

ShowPasswordModal& Modals::GetShowPasswordModal()
{
	return game.GetMainWindow().GetModals().showPasswordModal;
}

ChangePasswordModal& Modals::GetChangePasswordModal()
{
	return game.GetMainWindow().GetModals().changePasswordModal;
}

ChangeFileModal& Modals::GetChangeFileModal()
{
	return game.GetMainWindow().GetModals().changeFileModal;
}

ChangeNameModal& Modals::GetChangeNameModal()
{
	return game.GetMainWindow().GetModals().changeNameModal;
}

DeleteEntryModal& Modals::GetDeleteEntryModal()
{
	return game.GetMainWindow().GetModals().deleteEntryModal;
}

ErrorModal& Modals::GetErrorModal()
{
	return game.GetMainWindow().GetModals().errorModal;
}
