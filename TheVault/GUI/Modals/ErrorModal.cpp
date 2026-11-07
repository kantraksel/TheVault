#include "ErrorModal.hpp"
#include "Utils/ImGuiUtils.h"

ErrorModal::ErrorModal() : BaseModal("Error")
{
}

void ErrorModal::Open(const std::string_view& message)
{
	openNextFrame = true;
	this->message = message;
}

void ErrorModal::Open()
{
}

void ErrorModal::OnRender()
{
	Text(message);
	if (ImGui::Button("Cancel"))
	{
		message.clear();
		ImGui::CloseCurrentPopup();
	}
}
