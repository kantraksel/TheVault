#include "ShowPasswordWarningModal.hpp"
#include "ShowPasswordModal.hpp"
#include "Utils/ImGuiUtils.h"

ShowPasswordWarningModal::ShowPasswordWarningModal(ShowPasswordModal& showPasswordModal) : BaseIndexModal("Password Details - Warning"), showPasswordModal(showPasswordModal)
{
}

void ShowPasswordWarningModal::OnRender()
{
	Text("Are you sure you want to show the password?\nBe sure to not expose it via screen sharing!");
	if (ImGui::Button("Yes"))
	{
		Close();
		showPasswordModal.Open(selectedIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("No"))
		Close();
}
