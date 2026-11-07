#include "ShowPasswordWarningModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "../Modals.hpp"

ShowPasswordWarningModal::ShowPasswordWarningModal() : BaseIndexModal("Password Details - Warning")
{
}

void ShowPasswordWarningModal::OnRender()
{
	Text("Are you sure you want to show the password?\nBe sure to not expose it via screen sharing!");
	if (ImGui::Button("Yes"))
	{
		ImGui::CloseCurrentPopup();
		Modals::GetShowPasswordModal().Open(selectedIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("No"))
		ImGui::CloseCurrentPopup();
}
