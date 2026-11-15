#include "ConfirmExitModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

ConfirmExitModal::ConfirmExitModal() : BaseModal("Confirm Exit"), closeAppOnConfirmExit{}
{
}

void ConfirmExitModal::Open(bool closeAppOnConfirm)
{
	openNextFrame = true;
	closeAppOnConfirmExit = closeAppOnConfirm;
}

void ConfirmExitModal::Open()
{
	openNextFrame = true;
	closeAppOnConfirmExit = false;
}

void ConfirmExitModal::OnRender()
{
	Text("Do you want to save changes?");

	if (ImGui::Button("Yes"))
	{
		Close();
		game.GetVaultCommander().SaveVault(true, closeAppOnConfirmExit);
	}
	ImGui::SameLine();
	if (ImGui::Button("No"))
	{
		Close();
		game.GetVaultCommander().CloseVault(closeAppOnConfirmExit);
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		Close();
}
