#include "ErrorModal.hpp"
#include "Engine/GhostFries.h"
#include "Utils/ImGuiUtils.h"

ErrorModal::ErrorModal() : BaseModal("Error"), isErrorCritical{false}
{
}

void ErrorModal::Open(const std::string_view& message, bool critical)
{
	openNextFrame = true;
	this->message = message;
	isErrorCritical = critical;
}

void ErrorModal::Open()
{
}

void ErrorModal::OnClose()
{
	message.clear();
}

void ErrorModal::OnRender()
{
	if (isErrorCritical)
	{
		Text("TheVault encountered critical error and needs to be closed:");
		TextWrapped(message);
		ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0, 0.6f, 0.6f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0, 0.7f, 0.7f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0, 0.8f, 0.8f)));
		if (ButtonRightAligned("Exit"))
			GhostFries::Quit();
		ImGui::PopStyleColor(3);
		return;
	}

	TextWrapped(message);
	if (ButtonRightAligned("Close"))
		Close();
}
