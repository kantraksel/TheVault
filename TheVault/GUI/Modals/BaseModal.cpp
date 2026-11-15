#include "BaseModal.hpp"
#include <imgui.h>

BaseModal::BaseModal(const char* modalId) : modalId{modalId}, openNextFrame{false}, firstFrameDraw{false}, closeNextFrame{false}
{
}

void BaseModal::Open()
{
	openNextFrame = true;
	closeNextFrame = false;
}

void BaseModal::Close()
{
	openNextFrame = false;
	closeNextFrame = true;
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

		if (closeNextFrame)
		{
			closeNextFrame = false;
			ImGui::CloseCurrentPopup();
			OnClose();
		}
		ImGui::EndPopup();
	}

	if (firstFrameDraw)
		firstFrameDraw = false;
}

void BaseModal::OnOpen()
{
}

void BaseModal::OnClose()
{
}
