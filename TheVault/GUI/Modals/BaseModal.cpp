#include "BaseModal.hpp"
#include <imgui.h>

BaseModal::BaseModal(const char* modalId) : modalId{modalId}, openNextFrame{false}, firstFrameDraw{false}
{
}

void BaseModal::Open()
{
	openNextFrame = true;
}

void BaseModal::Render()
{
	if (openNextFrame)
	{
		openNextFrame = false;
		firstFrameDraw = true;
		ImGui::OpenPopup(modalId);

		OnOpen();
	}

	if (ImGui::BeginPopupModal(modalId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		OnRender();
		ImGui::EndPopup();
	}

	if (firstFrameDraw)
		firstFrameDraw = false;
}

void BaseModal::OnOpen()
{
}
